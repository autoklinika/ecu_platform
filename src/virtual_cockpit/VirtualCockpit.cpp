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

void VirtualCockpit::readDTC()
{
    pushCommand(CommandType::ReadDTC);
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
        runtime.ecuReady = false;

        runtime.dtcBusy = false;
        runtime.dtcReady = false;
        runtime.dtcError = msg;
        runtime.dtcs.clear();
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
    auto resetRuntime = [this]() {
        std::lock_guard<std::mutex> lock(runtimeMutex);
        runtime.vin.clear();
        runtime.sw.clear();
        runtime.hw.clear();
        runtime.lastError.clear();
        runtime.ecuReady = false;

        runtime.dtcBusy = false;
        runtime.dtcReady = false;
        runtime.dtcError.clear();
        runtime.dtcs.clear();
    };

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
            resetRuntime();
            state = State::Connecting;

            if(openStack())
                state = State::Connected;
            else
                setError("Stack open failed");
        }
        else if(cmd.type == CommandType::Disconnect)
        {
            state = State::Configured;
            closeStack();
            resetRuntime();
        }
        else if(cmd.type == CommandType::ReadDTC)
        {
            if(state != State::Connected || !udsCore)
                continue;

            if(!runtime.ecuReady)
                continue;

            if(sacDtc && sacDtc->isBusy())
                continue;

            {
                std::lock_guard<std::mutex> lock(runtimeMutex);
                runtime.dtcBusy = true;
                runtime.dtcReady = false;
                runtime.dtcError.clear();
                runtime.dtcs.clear();
            }

            sacDtc = std::make_unique<SAC_DTC_Module>(*udsCore);
            sacDtc->startRead();
        }
    }
}

void VirtualCockpit::engineTick()
{
    auto resetRuntime = [this]() {
        std::lock_guard<std::mutex> lock(runtimeMutex);
        runtime.vin.clear();
        runtime.sw.clear();
        runtime.hw.clear();
        runtime.lastError.clear();
        runtime.ecuReady = false;

        runtime.dtcBusy = false;
        runtime.dtcReady = false;
        runtime.dtcError.clear();
        runtime.dtcs.clear();
    };

    if(state == State::Connected)
    {
        if(duration_cast<milliseconds>(
            steady_clock::now() - lastFrameTime) > CAN_TIMEOUT)
        {
            logger.log("CAN timeout -> reconnect");
            resetRuntime();
            state = State::Reconnecting;
        }
    }

    if(state == State::Reconnecting)
    {
        closeStack();

        std::this_thread::sleep_for(milliseconds(200));

        resetRuntime();

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
        for(int i = 0; i < 3; i++)
            isotp->update();

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
                runtime.lastError.clear();
            }
            sac.reset();
        }
        else if(sac->hasError())
        {
            setError("SAC identification failed");
            sac.reset();
        }
    }

    if(sacDtc)
    {
        sacDtc->update();

        if(sacDtc->isDone())
        {
            auto dtcs = sacDtc->getDTCs();

            std::lock_guard<std::mutex> lock(runtimeMutex);
            runtime.dtcs.clear();
            runtime.dtcs.reserve(dtcs.size());

            for (const auto& d : dtcs)
            {
                DTCRecord rec;
                rec.code = d.code;
                rec.status = d.status;
                runtime.dtcs.push_back(rec);
            }

            runtime.dtcBusy = false;
            runtime.dtcReady = true;
            runtime.dtcError.clear();

            sacDtc.reset();
        }
        else if(sacDtc->hasError())
        {
            std::lock_guard<std::mutex> lock(runtimeMutex);
            runtime.dtcBusy = false;
            runtime.dtcReady = false;
            runtime.dtcError = sacDtc->getError();
            runtime.dtcs.clear();

            sacDtc.reset();
        }
    }
}

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

    sacDtc.reset();
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
