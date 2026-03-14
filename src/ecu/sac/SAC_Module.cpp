#include "ecu/sac/SAC_Module.h"

SAC_Module::SAC_Module(UDS_Core& core)
    : core_(core)
{
}

void SAC_Module::startIdentification()
{
    vin_.clear();
    sw_.clear();
    hw_.clear();
    error_.clear();

    core_.reset();
    step_ = Step::RequestVIN;
}

void SAC_Module::fail(const std::string& msg)
{
    error_ = msg;
    step_ = Step::Error;
}

bool SAC_Module::parseDidStringResponse(const std::vector<uint8_t>& resp,
                                        uint16_t expectedDid,
                                        std::string& out)
{
    if (resp.size() < 3)
        return false;

    if (resp[0] != 0x62)
        return false;

    const uint16_t did =
        (static_cast<uint16_t>(resp[1]) << 8) |
        static_cast<uint16_t>(resp[2]);

    if (did != expectedDid)
        return false;

    out.assign(resp.begin() + 3, resp.end());
    return true;
}

void SAC_Module::update()
{
    switch (step_)
    {
    case Step::Idle:
    case Step::Done:
    case Step::Error:
        return;

    case Step::RequestVIN:
        if (!core_.isIdle())
            return;

        if (core_.request({0x22, 0xF1, 0x90}))
            step_ = Step::WaitVIN;
        else
            fail("Cannot send VIN request");
        return;

    case Step::WaitVIN:
        if (core_.getState() == UDS_Core::State::Done)
        {
            const auto resp = core_.getResponse();
            core_.reset();

            if (parseDidStringResponse(resp, 0xF190, vin_))
                step_ = Step::RequestSW;
            else
                fail("Invalid VIN response");
        }
        else if (core_.getState() == UDS_Core::State::Error ||
                 core_.getState() == UDS_Core::State::Timeout)
        {
            core_.reset();
            fail("VIN read timeout/error");
        }
        return;

    case Step::RequestSW:
        if (!core_.isIdle())
            return;

        if (core_.request({0x22, 0xF1, 0x88}))
            step_ = Step::WaitSW;
        else
            fail("Cannot send SW request");
        return;

    case Step::WaitSW:
        if (core_.getState() == UDS_Core::State::Done)
        {
            const auto resp = core_.getResponse();
            core_.reset();

            if (parseDidStringResponse(resp, 0xF188, sw_))
                step_ = Step::RequestHW;
            else
                fail("Invalid SW response");
        }
        else if (core_.getState() == UDS_Core::State::Error ||
                 core_.getState() == UDS_Core::State::Timeout)
        {
            core_.reset();
            fail("SW read timeout/error");
        }
        return;

    case Step::RequestHW:
        if (!core_.isIdle())
            return;

        if (core_.request({0x22, 0xF1, 0x92}))
            step_ = Step::WaitHW;
        else
            fail("Cannot send HW request");
        return;

    case Step::WaitHW:
        if (core_.getState() == UDS_Core::State::Done)
        {
            const auto resp = core_.getResponse();
            core_.reset();

            if (parseDidStringResponse(resp, 0xF192, hw_))
                step_ = Step::Done;
            else
                fail("Invalid HW response");
        }
        else if (core_.getState() == UDS_Core::State::Error ||
                 core_.getState() == UDS_Core::State::Timeout)
        {
            core_.reset();
            fail("HW read timeout/error");
        }
        return;
    }
}

bool SAC_Module::isReady() const
{
    return step_ == Step::Done;
}

bool SAC_Module::hasError() const
{
    return step_ == Step::Error;
}

std::string SAC_Module::getVIN() const
{
    return vin_;
}

std::string SAC_Module::getSW() const
{
    return sw_;
}

std::string SAC_Module::getHW() const
{
    return hw_;
}

std::string SAC_Module::getError() const
{
    return error_;
}
