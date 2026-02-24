#include "UDS_Core.h"
#include "isotp/ISOTP.h"

using namespace std::chrono;

UDS_Core::UDS_Core(ISOTP& isotp)
    : isotp_(isotp)
{
    testerPresentTimer_ = steady_clock::now();
}

bool UDS_Core::request(const std::vector<uint8_t>& payload)
{
    if(state_ != State::Idle)
        return false;

    if(!isotp_.send(payload))
        return false;

    state_ = State::WaitingResponse;
    requestStart_ = steady_clock::now();

    return true;
}

void UDS_Core::update()
{
    // ISO-TP tick (nie blokuje)
     
    auto now = steady_clock::now();

    if(state_ == State::WaitingResponse)
    {
        if(isotp_.hasMessage())
        {
            auto resp = isotp_.receive();

            // Negative Response
            if(resp.size() >= 3 && resp[0] == 0x7F)
            {
                // NRC 0x78 = Response Pending
                if(resp[2] == 0x78)
                {
                    requestStart_ = now;
                    return;
                }

                state_ = State::Error;
                response_ = resp;
                return;
            }

            response_ = resp;
            state_ = State::Done;
            return;
        }

        // Timeout
        if(duration_cast<milliseconds>(now - requestStart_) > RESPONSE_TIMEOUT)
        {
            state_ = State::Timeout;
            return;
        }
    }

    handleTesterPresent();
}

void UDS_Core::handleTesterPresent()
{
    if(!sessionActive_)
        return;

    if(state_ != State::Idle)
        return;

    auto now = steady_clock::now();

    if(duration_cast<milliseconds>(now - testerPresentTimer_) <
       TESTER_PRESENT_PERIOD)
        return;

    std::vector<uint8_t> testerPresent = {0x3E, 0x80}; // suppress positive

    isotp_.send(testerPresent);

    testerPresentTimer_ = now;
}

bool UDS_Core::isIdle() const
{
    return state_ == State::Idle;
}

UDS_Core::State UDS_Core::getState() const
{
    return state_;
}

std::vector<uint8_t> UDS_Core::getResponse()
{
    state_ = State::Idle;
    return response_;
}

void UDS_Core::reset()
{
    state_ = State::Idle;
    response_.clear();
}

void UDS_Core::setSessionActive(bool active)
{
    sessionActive_ = active;
    testerPresentTimer_ = steady_clock::now();
}