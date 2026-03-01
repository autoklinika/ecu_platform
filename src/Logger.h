#pragma once
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

class Logger
{
public:
    Logger()
    {
        file.open("/home/pi/ecu_logs/ecu.log", std::ios::app);
    }

    void log(const std::string& msg)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if(!file.is_open())
            return;

        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);

        file << std::put_time(std::localtime(&t), "%F %T")
             << " | " << msg << "\n";

        file.flush();
    }

private:
    std::ofstream file;
    std::mutex mutex;
};