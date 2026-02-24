#include "VirtualCockpit.h"
#include <iostream>
#include <cstring>


using namespace std::chrono;

// ========================
// Constructor / Destructor
// ========================

VirtualCockpit::VirtualCockpit()
{
}

VirtualCockpit::~VirtualCockpit()
{
    stop();
}

// ========================
// Public API
// ========================

void VirtualCockpit::start()
{
    if(running)
        return;

    running = true;
    engineThread = std::thread(&VirtualCockpit::engineLoop, this);
}

void VirtualCockpit::stop()
{
    if(!running)
        return;

    pushCommand(CommandType::Stop);

    if(engineThread.joinable())
        engineThread.join();
}

VirtualCockpit::State VirtualCockpit::getState() const
{
    return state.load();
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

bool VirtualCockpit::selectECU(const std::string& ecuName)
{
    if(state == State::Connected)
        return false;

    selectedECU = ecuName;
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

// ========================
// Engine Loop
// ========================

void VirtualCockpit::engineLoop()
{
    while(running)
    {
        auto cycleStart = steady_clock::now();

        processCommands();
        engineTick();

        std::this_thread::sleep_until(cycleStart + CYCLE_TIME);
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

        switch(cmd.type)
        {
            case CommandType::Connect:
                if(state == State::Configured)
                {
                    state = State::Connecting;

                    if(openStack())
                        state = State::Connected;
                    else
                        state = State::Error;
                }
                break;

            case CommandType::Disconnect:
                if(state == State::Connected)
                {
                    closeStack();
                    state = State::Configured;
                }
                break;

            case CommandType::Stop:
                running = false;
                break;
        }
    }
}

void VirtualCockpit::engineTick()
{
    if(state != State::Connected)
        return;

    Frame f;

    // 1️⃣ Przetwarzamy wszystkie ramki
    while(frameQueue && frameQueue->tryPop(f))
    {
        dispatcher->dispatch(f.id, f.data, f.len);
    }

    // 2️⃣ Dajemy ISO-TP kilka cykli (ważne!)
    if(isotp)
    {
        for(int i = 0; i < 3; ++i)
            isotp->update();
    }

    // 3️⃣ Dopiero teraz warstwa UDS
    if(udsCore)
        udsCore->update();

    // 4️⃣ Na końcu ECU logic
    if(sac)
    {
        sac->update();

        if(sac->isReady())
        {
            std::cout << "\n=== SAC IDENTIFICATION ===\n";
            std::cout << "VIN: " << sac->getVIN() << "\n";
            std::cout << "SW : " << sac->getSW()  << "\n";
            std::cout << "HW : " << sac->getHW()  << "\n";
            std::cout << "==========================\n";

            sac.reset();
        }

        if(sac && sac->hasError())
        {
            std::cout << "SAC identification ERROR\n";
            sac.reset();
        }
    }
}

// ========================
// Stack Control
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
    // ===== ECU Module =====
    if (selectedECU == "SAC")
    {
        sac = std::make_unique<SAC_Module>(*udsCore);
        sac->startIdentification();
    }

    rxThread = std::thread(&VirtualCockpit::rxLoop, this);

    return true;
}

void VirtualCockpit::closeStack()
{   
    sac.reset();
    
    if(transport)
    {
        transport->close();
    }

    if(rxThread.joinable())
        rxThread.join();

    udsCore.reset();
    isotp.reset();
    dispatcher.reset();
    frameQueue.reset();
    transport.reset();
}

void VirtualCockpit::rxLoop()
{
    while(running && state == State::Connected)
    {
        uint32_t id;
        uint8_t data[8];
        uint8_t len;

        if(transport->receiveFrame(id, data, len))
        {
            Frame f{};
            f.id = id;
            f.len = len;
            std::memcpy(f.data, data, len);

            frameQueue->push(f);
        }
    }
}

// ========================

void VirtualCockpit::pushCommand(CommandType type)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    commandQueue.push({type});
}