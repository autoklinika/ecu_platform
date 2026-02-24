#include "VirtualCockpit.h"
#include <iostream>
#include <thread>

VirtualCockpit::VirtualCockpit()
{
}

VirtualCockpit::~VirtualCockpit()
{
    stop();
}

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

    state = State::Stopped;
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

void VirtualCockpit::pushCommand(CommandType type)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    commandQueue.push({type});
}

void VirtualCockpit::engineLoop()
{
    using clock = std::chrono::steady_clock;

    while(running)
    {
        auto cycleStart = clock::now();

        processCommands();
        engineTick();

        std::this_thread::sleep_until(cycleStart + CYCLE_TIME);
    }

    // Clean shutdown
    if(canOpen)
        closeCAN();
}

void VirtualCockpit::processCommands()
{
    std::queue<Command> localQueue;

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        std::swap(localQueue, commandQueue);
    }

    while(!localQueue.empty())
    {
        auto cmd = localQueue.front();
        localQueue.pop();

        switch(cmd.type)
        {
            case CommandType::Connect:
                if(state == State::Configured)
                {
                    state = State::Connecting;

                    if(openCAN())
                        state = State::Connected;
                    else
                        state = State::Error;
                }
                break;

            case CommandType::Disconnect:
                if(state == State::Connected)
                {
                    closeCAN();
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
    if(state == State::Connected)
    {
        // 🔥 tutaj będzie:
        // transport.update();
        // dispatcher.update();
        // isotp.update();
        // uds.update();
        // scheduler.update();
    }
}

bool VirtualCockpit::openCAN()
{
    canTransport = std::make_unique<Transport_CAN_Linux>();

    if(!canTransport->open(canInterface, canBitrate))
        return false;

    canOpen = true;
    return true;
}

void VirtualCockpit::closeCAN()
{
    if(canTransport)
    {
        canTransport->close();
        canTransport.reset();
    }

    canOpen = false;
}