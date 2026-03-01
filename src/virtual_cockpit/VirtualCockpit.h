#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <memory>
#include <queue>
#include <chrono>

#include "transport/Transport_CAN_Linux.h"
#include "core/FrameQueue.h"
#include "core/CAN_Dispatcher.h"
#include "isotp/ISOTP.h"
#include "uds/UDS_Core.h"
#include "ecu/sac/SAC_Module.h"
#include "Logger.h"

class VirtualCockpit
{
public:
    VirtualCockpit();
    ~VirtualCockpit();

    void start();
    void stop();

    bool configureCAN(const std::string& iface, int bitrate);
    bool selectECU(const std::string& ecu);

    void connect();
    void disconnect();

    enum class State
    {
        Idle,
        Configured,
        Connecting,
        Connected,
        Reconnecting,
        Error
    };

    State getState() const;

    struct RuntimeData
    {
        std::string vin;
        std::string sw;
        std::string hw;
        std::string lastError;
        bool ecuReady = false;
    };

    RuntimeData getRuntime() const;

private:
    void engineLoop();
    void engineTick();
    void processCommands();

    bool openStack();
    void closeStack();
    void rxLoop();

    void setError(const std::string& msg);

    static constexpr std::chrono::milliseconds CYCLE_TIME{5};
    static constexpr std::chrono::milliseconds CAN_TIMEOUT{1000};

    std::thread engineThread;
    std::thread rxThread;

    std::atomic<bool> running{false};
    std::atomic<State> state{State::Idle};

    std::string canInterface;
    int canBitrate = 0;
    std::string selectedECU;

    std::unique_ptr<Transport_CAN_Linux> transport;
    std::unique_ptr<FrameQueue> frameQueue;
    std::unique_ptr<CAN_Dispatcher> dispatcher;
    std::unique_ptr<ISOTP> isotp;
    std::unique_ptr<UDS_Core> udsCore;
    std::unique_ptr<SAC_Module> sac;

    std::chrono::steady_clock::time_point lastFrameTime;
    int reconnectAttempts = 0;

    RuntimeData runtime;
    mutable std::mutex runtimeMutex;

    Logger logger;

    enum class CommandType { Connect, Disconnect, Stop };
    struct Command { CommandType type; };

    std::queue<Command> commandQueue;
    std::mutex queueMutex;

    void pushCommand(CommandType t);
};