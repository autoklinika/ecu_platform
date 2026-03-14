#include "ecu/sac/SAC_DTC_Module.h"

SAC_DTC_Module::SAC_DTC_Module(UDS_Core& core)
    : core_(core)
{
}

void SAC_DTC_Module::resetResult()
{
    dtcs_.clear();
    error_.clear();
}

void SAC_DTC_Module::fail(const std::string& msg)
{
    error_ = msg;
    step_ = Step::Error;
}

void SAC_DTC_Module::startRead()
{
    if (isBusy())
        return;

    resetResult();
    op_ = Op::Read;
    step_ = Step::RequestSession;
    core_.reset();
}

void SAC_DTC_Module::startClear()
{
    if (isBusy())
        return;

    resetResult();
    op_ = Op::Clear;
    step_ = Step::RequestSession;
    core_.reset();
}

bool SAC_DTC_Module::isBusy() const
{
    return step_ != Step::Idle &&
           step_ != Step::Done &&
           step_ != Step::Error;
}

bool SAC_DTC_Module::isDone() const
{
    return step_ == Step::Done;
}

bool SAC_DTC_Module::hasError() const
{
    return step_ == Step::Error;
}

const std::vector<SAC_DTC_Module::DTCItem>& SAC_DTC_Module::getDTCs() const
{
    return dtcs_;
}

std::string SAC_DTC_Module::getError() const
{
    return error_;
}

void SAC_DTC_Module::parseReadResponse(const std::vector<uint8_t>& resp)
{
    dtcs_.clear();

    if (resp.size() < 3 || resp[0] != 0x59 || resp[1] != 0x02)
    {
        fail("Invalid DTC response");
        return;
    }

    std::size_t pos = 3; // 59 02 statusMask

    while (pos + 3 < resp.size())
    {
        DTCItem item;
        item.code =
            (static_cast<uint32_t>(resp[pos]) << 16) |
            (static_cast<uint32_t>(resp[pos + 1]) << 8) |
            static_cast<uint32_t>(resp[pos + 2]);

        item.status = resp[pos + 3];
        dtcs_.push_back(item);
        pos += 4;
    }

    step_ = Step::Done;
}

void SAC_DTC_Module::update()
{
    switch (step_)
    {
    case Step::Idle:
    case Step::Done:
    case Step::Error:
        return;

    case Step::RequestSession:
        if (!core_.isIdle())
            return;

        // 10 03 = extended diagnostic session
        if (core_.request({0x10, 0x03}))
            step_ = Step::WaitSession;
        else
            fail("Cannot send session request");
        return;

    case Step::WaitSession:
        if (core_.getState() == UDS_Core::State::Done)
        {
            const auto resp = core_.getResponse();
            if (resp.size() >= 2 && resp[0] == 0x50 && resp[1] == 0x03)
            {
                core_.reset();
                step_ = (op_ == Op::Read) ? Step::RequestRead : Step::RequestClear;
            }
            else
            {
                core_.reset();
                fail("Extended session rejected");
            }
        }
        else if (core_.getState() == UDS_Core::State::Error ||
                 core_.getState() == UDS_Core::State::Timeout)
        {
            core_.reset();
            fail("Extended session timeout/error");
        }
        return;

    case Step::RequestRead:
        if (!core_.isIdle())
            return;

        // 19 02 FF = report DTC by status mask
        if (core_.request({0x19, 0x02, 0xFF}))
            step_ = Step::WaitRead;
        else
            fail("Cannot send DTC read request");
        return;

    case Step::WaitRead:
        if (core_.getState() == UDS_Core::State::Done)
        {
            auto resp = core_.getResponse();
            core_.reset();

            if (resp.size() >= 3 && resp[0] == 0x7F && resp[2] == 0x78)
                return;

            parseReadResponse(resp);
        }
        else if (core_.getState() == UDS_Core::State::Error ||
                 core_.getState() == UDS_Core::State::Timeout)
        {
            core_.reset();
            fail("DTC read timeout/error");
        }
        return;

    case Step::RequestClear:
        if (!core_.isIdle())
            return;

        // 14 FF FF FF = clear all DTCs
        if (core_.request({0x14, 0xFF, 0xFF, 0xFF}))
            step_ = Step::WaitClear;
        else
            fail("Cannot send DTC clear request");
        return;

    case Step::WaitClear:
        if (core_.getState() == UDS_Core::State::Done)
        {
            auto resp = core_.getResponse();
            core_.reset();

            if (!resp.empty() && resp[0] == 0x54)
            {
                dtcs_.clear();
                step_ = Step::Done;
            }
            else
            {
                fail("DTC clear rejected");
            }
        }
        else if (core_.getState() == UDS_Core::State::Error ||
                 core_.getState() == UDS_Core::State::Timeout)
        {
            core_.reset();
            fail("DTC clear timeout/error");
        }
        return;
    }
}
