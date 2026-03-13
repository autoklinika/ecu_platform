#pragma once

#include "IECU_Module.h"
#include "uds/UDS_Core.h"

#include <string>

class SAC_Module : public IECU_Module
{
public:
    explicit SAC_Module(UDS_Core& core);

    void startIdentification() override;
    void update() override;

    bool isReady() const override;
    bool hasError() const override;

    std::string getVIN() const override;
    std::string getSW()  const override;
    std::string getHW()  const override;

private:

    enum class State
{
    Idle,

    RequestSession,
    WaitSession,

    RequestVIN,
    WaitVIN,

    RequestSW,
    WaitSW,

    RequestHW,
    WaitHW,

    Done,
    Error
};

    UDS_Core& uds;
    State state = State::Idle;

    std::string vin;
    std::string sw;
    std::string hw;

    void requestDID(uint16_t did);
    void processResponse(uint16_t expectedDid);
};