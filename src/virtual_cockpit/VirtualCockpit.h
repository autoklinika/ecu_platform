#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <chrono>
#include <memory>

#include "transport/Transport_CAN_Linux.h"
#include "core/FrameQueue.h"
#include "core/CAN_Dispatcher.h"
#include "isotp/ISOTP.h"
#include "uds/UDS_Core.h"
#include "SAC_Module.h"

class VirtualCockpit
{
public:
    VirtualCockpit();
    ~VirtualCockpit();

    // ===== Lifecycle =====
    void start();
    void stop();

    bool configureCAN(const std::string& iface, int bitrate);
    bool selectECU(const std::string& ecuName);

    void connect();
    void disconnect();

    enum class State
    {
        Idle,
        Configured,
        Connecting,
        Connected,
        Error
    };

    State getState() const;

private:

    // ===== Engine =====
    void engineLoop();
    void processCommands();
    void engineTick();

    static constexpr std::chrono::milliseconds CYCLE_TIME{5};

    std::thread engineThread;
    std::thread rxThread;

    std::atomic<bool> running{false};
    std::atomic<State> state{State::Idle};

    // ===== Configuration =====
    std::string canInterface;
    int canBitrate = 0;
    std::string selectedECU;

    // ===== Transport & Stack =====
    std::unique_ptr<Transport_CAN_Linux> transport;
    std::unique_ptr<FrameQueue> frameQueue;
    std::unique_ptr<CAN_Dispatcher> dispatcher;

    std::unique_ptr<ISOTP> isotp;
    std::unique_ptr<UDS_Core> udsCore;
    std::unique_ptr<SAC_Module> sac;

    // ===== Command System =====
    enum class CommandType
    {
        Connect,
        Disconnect,
        Stop
    };

    struct Command
    {
        CommandType type;
    };

    std::queue<Command> commandQueue;
    std::mutex queueMutex;

    void pushCommand(CommandType type);

    // ===== Internal helpers =====
    bool openStack();
    void closeStack();
    void rxLoop();
};