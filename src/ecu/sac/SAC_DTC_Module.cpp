#include "SAC_DTC_Module.h"

SAC_DTC_Module::SAC_DTC_Module(UDS_Core& core)
    : uds_(core)
{
}

void SAC_DTC_Module::startRead()
{
    if (state_ != State::Idle)
        return;

    dtcs_.clear();
    error_.clear();

    uds_.setSessionActive(false);
    uds_.reset();

    sessionActive_ = false;
    state_ = State::RequestSession;
}

void SAC_DTC_Module::resetToIdle()
{
    dtcs_.clear();
    error_.clear();

    sessionActive_ = false;

    uds_.setSessionActive(false);
    uds_.reset();

    state_ = State::Idle;
}

void SAC_DTC_Module::update()
{
    if (state_ == State::Idle ||
        state_ == State::Done ||
        state_ == State::Error)
        return;

    switch (state_)
    {
    case State::RequestSession:
        if (uds_.isIdle())
        {
            if (uds_.request({0x10, 0x03}))
                state_ = State::WaitSession;
            else
            {
                error_ = "Cannot send extended session request";
                state_ = State::Error;
            }
        }
        break;

    case State::WaitSession:
        if (uds_.getState() == UDS_Core::State::Done)
        {
            auto resp = uds_.getResponse();

            if (resp.size() >= 2 &&
                resp[0] == 0x50 &&
                resp[1] == 0x03)
            {
                uds_.setSessionActive(true);
                sessionActive_ = true;

                uds_.reset();
                state_ = State::RequestRead;
            }
            else
            {
                uds_.setSessionActive(false);
                sessionActive_ = false;
                uds_.reset();

                error_ = "Extended session rejected";
                state_ = State::Error;
            }
        }
        else if (uds_.getState() == UDS_Core::State::Error ||
                 uds_.getState() == UDS_Core::State::Timeout)
        {
            uds_.setSessionActive(false);
            sessionActive_ = false;
            uds_.reset();

            error_ = "Extended session timeout/error";
            state_ = State::Error;
        }
        break;

    case State::RequestRead:
        if (uds_.isIdle())
        {
            if (uds_.request({0x19, 0x02, 0xFF}))
                state_ = State::WaitRead;
            else
            {
                uds_.setSessionActive(false);
                sessionActive_ = false;

                error_ = "Cannot send DTC read request";
                state_ = State::Error;
            }
        }
        break;

    case State::WaitRead:
        if (uds_.getState() == UDS_Core::State::Done)
        {
            auto resp = uds_.getResponse();

            if (parseDTCResponse(resp))
            {
                uds_.reset();
                state_ = State::Done;
            }
            else
            {
                uds_.setSessionActive(false);
                sessionActive_ = false;
                uds_.reset();

                error_ = "Invalid DTC response";
                state_ = State::Error;
            }
        }
        else if (uds_.getState() == UDS_Core::State::Error ||
                 uds_.getState() == UDS_Core::State::Timeout)
        {
            uds_.setSessionActive(false);
            sessionActive_ = false;
            uds_.reset();

            error_ = "DTC read timeout/error";
            state_ = State::Error;
        }
        break;

    default:
        break;
    }
}

bool SAC_DTC_Module::parseDTCResponse(const std::vector<uint8_t>& resp)
{
    // 59 02 <statusAvailabilityMask> [DTC_H DTC_M DTC_L STATUS]...
    if (resp.size() < 3)
        return false;

    if (resp[0] != 0x59 || resp[1] != 0x02)
        return false;

    dtcs_.clear();

    size_t pos = 3;

    while (pos + 3 < resp.size())
    {
        DTCRecord rec;
        rec.code =
            (static_cast<uint32_t>(resp[pos]) << 16) |
            (static_cast<uint32_t>(resp[pos + 1]) << 8) |
            static_cast<uint32_t>(resp[pos + 2]);
        rec.status = resp[pos + 3];

        dtcs_.push_back(rec);
        pos += 4;
    }

    return true;
}

bool SAC_DTC_Module::isDone() const
{
    return state_ == State::Done;
}

bool SAC_DTC_Module::hasError() const
{
    return state_ == State::Error;
}

bool SAC_DTC_Module::isBusy() const
{
    return state_ != State::Idle &&
           state_ != State::Done &&
           state_ != State::Error;
}

bool SAC_DTC_Module::isIdle() const
{
    return state_ == State::Idle;
}

std::vector<SAC_DTC_Module::DTCRecord> SAC_DTC_Module::getDTCs() const
{
    return dtcs_;
}

std::string SAC_DTC_Module::getError() const
{
    return error_;
}
