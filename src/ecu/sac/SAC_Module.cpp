#include "SAC_Module.h"

SAC_Module::SAC_Module(UDS_Core& core)
    : uds(core)
{
}

void SAC_Module::startIdentification()
{
    vin.clear();
    sw.clear();
    hw.clear();

    state = State::RequestSession;
}

void SAC_Module::update()
{
    if (state == State::Idle ||
        state == State::Done ||
        state == State::Error)
        return;

    switch (state)
    {
    // ==========================
    // 1️⃣ Extended Session
    // ==========================
    case State::RequestSession:
        if (uds.request({0x10, 0x03}))
            state = State::WaitSession;
        else
            state = State::Error;
        break;

    case State::WaitSession:
        if (uds.getState() == UDS_Core::State::Done)
        {
            auto resp = uds.getResponse();

            if (resp.size() >= 2 && resp[0] == 0x50)
                state = State::RequestVIN;
            else
                state = State::Error;
        }
        break;

    // ==========================
    // 2️⃣ VIN
    // ==========================
    case State::RequestVIN:
        if (uds.request({0x22, 0xF1, 0x90}))
            state = State::WaitVIN;
        else
            state = State::Error;
        break;

    case State::WaitVIN:
        if (uds.getState() == UDS_Core::State::Done)
        {
            auto resp = uds.getResponse();

            if (resp.size() > 3 && resp[0] == 0x62)
            {
                vin.assign(resp.begin() + 3, resp.end());
                state = State::RequestSW;
            }
            else
                state = State::Error;
        }
        break;

    // ==========================
    // 3️⃣ SW
    // ==========================
    case State::RequestSW:
        if (uds.request({0x22, 0xF1, 0x88}))
            state = State::WaitSW;
        else
            state = State::Error;
        break;

    case State::WaitSW:
        if (uds.getState() == UDS_Core::State::Done)
        {  
            auto resp = uds.getResponse();

            if (resp.size() > 3 && resp[0] == 0x62)
            {
                sw.assign(resp.begin() + 3, resp.end());
                state = State::RequestHW;
            }
            else
                state = State::Error;
        }
        break;

    // ==========================
    // 4️⃣ HW
    // ==========================
    case State::RequestHW:
        if (uds.request({0x22, 0xF1, 0x92}))
            state = State::WaitHW;
        else
            state = State::Error;
        break;

    case State::WaitHW:
    
        if (uds.getState() == UDS_Core::State::Done)
        {
            auto resp = uds.getResponse();
            
            if (resp.size() > 3 && resp[0] == 0x62)
            {
                hw.assign(resp.begin() + 3, resp.end());
                state = State::Done;
            }
            else
                state = State::Error;
        }
        break;
    }
}

bool SAC_Module::isReady() const
{
    return state == State::Done;
}

bool SAC_Module::hasError() const
{
    return state == State::Error;
}

std::string SAC_Module::getVIN() const { return vin; }
std::string SAC_Module::getSW()  const { return sw;  }
std::string SAC_Module::getHW()  const { return hw;  }