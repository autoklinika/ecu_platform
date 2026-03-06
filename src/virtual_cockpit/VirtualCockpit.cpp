#include "VirtualCockpit.h"
#include <cstring>
#include <thread>

using namespace std::chrono;

VirtualCockpit::VirtualCockpit() {}

VirtualCockpit::~VirtualCockpit()
{
    stop();
}

void VirtualCockpit::start()
{
    if(running) return;
    running = true;
    engineThread = std::thread(&VirtualCockpit::engineLoop, this);
}

void VirtualCockpit::stop()
{
    if(!running) return;
    pushCommand(CommandType::Stop);
    if(engineThread.joinable())
        engineThread.join();
}

VirtualCockpit::State VirtualCockpit::getState() const
{
    return state.load();
}

VirtualCockpit::RuntimeData VirtualCockpit::getRuntime() const
{
    std::lock_guard<std::mutex> lock(runtimeMutex);
    return runtime;
}

bool VirtualCockpit::configureCAN(const std::string& iface, int bitrate)
{
    if(state != State::Idle && state != State::Configured)
        return false;

    canInterface = iface;
    canBitrate = bitrate;
    state = State::Configured;
    return true;
}

bool VirtualCockpit::selectECU(const std::string& ecu)
{
    selectedECU = ecu;
    return true;
}

void VirtualCockpit::connect()
{
    pushCommand(CommandType::Connect);
}

void VirtualCockpit::disconnect()
{
    pushCommand(CommandType::Disconnect);
}

void VirtualCockpit::pushCommand(CommandType t)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    commandQueue.push({t});
}

void VirtualCockpit::setError(const std::string& msg)
{
    logger.log("ERROR: " + msg);

    {
        std::lock_guard<std::mutex> lock(runtimeMutex);
        runtime.lastError = msg;
    }

    state = State::Error;
}

void VirtualCockpit::engineLoop()
{
    while(running)
    {
        auto start = steady_clock::now();
        processCommands();
        engineTick();
        std::this_thread::sleep_until(start + CYCLE_TIME);
    }
    closeStack();
}

void VirtualCockpit::processCommands()
{
    std::queue<Command> local;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        std::swap(local, commandQueue);
    }

    while(!local.empty())
    {
        auto cmd = local.front();
        local.pop();

        if(cmd.type == CommandType::Stop)
        {
            running = false;
        }
        else if(cmd.type == CommandType::Connect &&
                state == State::Configured)
        {
            state = State::Connecting;

            if(openStack())
                state = State::Connected;
            else
                setError("Stack open failed");
        }
        else if(cmd.type == CommandType::Disconnect)
        {
            closeStack();
            state = State::Configured;
        }
    }
}

void VirtualCockpit::engineTick()
{
    if(state == State::Connected)
    {
        if(duration_cast<milliseconds>(
            steady_clock::now() - lastFrameTime) > CAN_TIMEOUT)
        {
            logger.log("CAN timeout -> reconnect");
            state = State::Reconnecting;
        }
    }

    if(state == State::Reconnecting)
    {
        closeStack();

        std::this_thread::sleep_for(milliseconds(200));

        if(openStack())
        {
            reconnectAttempts = 0;
            state = State::Connected;
        }
        else
        {
            reconnectAttempts++;
            if(reconnectAttempts > 5)
                setError("Reconnect failed");
        }
        return;
    }

    if(state != State::Connected)
        return;

    Frame f;
    while(frameQueue && frameQueue->tryPop(f))
    {
        lastFrameTime = steady_clock::now();
        dispatcher->dispatch(f.id, f.data, f.len);
    }

    if(isotp)
        for(int i=0;i<3;i++) isotp->update();

    if(udsCore)
        udsCore->update();

    if(sac)
    {
        sac->update();

        if(sac->isReady())
        {
            auto vin = sac->getVIN();
            if(vin.size() == 17)
            {
                std::lock_guard<std::mutex> lock(runtimeMutex);
                runtime.vin = vin;
                runtime.sw = sac->getSW();
                runtime.hw = sac->getHW();
                runtime.ecuReady = true;
            }
            sac.reset();
        }
        else if(sac->hasError())
        {
            setError("SAC identification failed");
            sac.reset();
        }
    }
}
// ========================
// STACK CONTROL
// ========================

bool VirtualCockpit::openStack()
{
    transport = std::make_unique<Transport_CAN_Linux>();

    if(!transport->open(canInterface, canBitrate))
        return false;

    frameQueue = std::make_unique<FrameQueue>();
    dispatcher = std::make_unique<CAN_Dispatcher>();

    uint32_t testerToEcu = 0x18DA30F9;
    uint32_t ecuToTester = 0x18DAF930;

    isotp = std::make_unique<ISOTP>(*transport, testerToEcu, ecuToTester);
    udsCore = std::make_unique<UDS_Core>(*isotp);

    dispatcher->registerHandler(isotp.get());

    if(selectedECU == "SAC")
    {
        sac = std::make_unique<SAC_Module>(*udsCore);
        sac->startIdentification();
    }

    lastFrameTime = std::chrono::steady_clock::now();

    rxThread = std::thread(&VirtualCockpit::rxLoop, this);

    return true;
}

void VirtualCockpit::closeStack()
{
    if(transport)
    {
        transport->close();
        transport.reset();
    }

    if(rxThread.joinable())
        rxThread.join();

    sac.reset();
    udsCore.reset();
    isotp.reset();
    dispatcher.reset();
    frameQueue.reset();
}

void VirtualCockpit::rxLoop()
{
    while(running && state == State::Connected)
    {
        uint32_t id;
        uint8_t data[8];
        uint8_t len;

        if(transport && transport->receiveFrame(id, data, len))
        {
            Frame f{};
            f.id = id;
            f.len = len;
            std::memcpy(f.data, data, len);

            if(frameQueue)
                frameQueue->push(f);
        }
    }
}
