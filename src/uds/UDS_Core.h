#pragma once

#include <vector>
#include <chrono>
#include <cstdint>

class ISOTP;

class UDS_Core
{
public:
    enum class State
    {
        Idle,
        WaitingResponse,
        Done,
        Error,
        Timeout
    };

    explicit UDS_Core(ISOTP& isotp);

    // Wywoływane cyklicznie w protocol thread
    void update();

    // Wysyła request UDS (bez PCI – czysty payload)
    bool request(const std::vector<uint8_t>& payload);

    bool isIdle() const;
    State getState() const;

    // Zwraca odpowiedź i ustawia stan Idle
    std::vector<uint8_t> getResponse();

    void reset();

    void setSessionActive(bool active);

private:
    void handleTesterPresent();

private:
    ISOTP& isotp_;

    State state_ = State::Idle;

    std::vector<uint8_t> response_;

    std::chrono::steady_clock::time_point requestStart_;
    std::chrono::steady_clock::time_point testerPresentTimer_;

    bool sessionActive_ = false;

    static constexpr std::chrono::milliseconds RESPONSE_TIMEOUT{3000};
    static constexpr std::chrono::milliseconds TESTER_PRESENT_PERIOD{2000};
};