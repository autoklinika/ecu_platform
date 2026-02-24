#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <chrono>
#include <memory>
#include "Transport_CAN_Linux.h"

std::unique_ptr<Transport_CAN_Linux> canTransport;

class VirtualCockpit
{
public:
    VirtualCockpit();
    ~VirtualCockpit();

    // ===== Lifecycle =====
    void start();
    void stop();

    // ===== Configuration =====
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
        Error,
        Stopped
    };

    State getState() const;

private:

    // ===== Engine =====
    void engineLoop();
    void processCommands();
    void engineTick();

    static constexpr std::chrono::milliseconds CYCLE_TIME{5};

    std::thread engineThread;
    std::atomic<bool> running{false};
    std::atomic<State> state{State::Idle};

    // ===== Configuration =====
    std::string canInterface;
    int canBitrate = 0;
    std::string selectedECU;

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
    bool openCAN();
    void closeCAN();

    bool canOpen = false;
};