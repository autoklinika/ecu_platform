#pragma once

#include "uds/UDS_Core.h"
#include <string>
#include <vector>
#include <cstdint>

class SAC_Module
{
public:
    explicit SAC_Module(UDS_Core& core);

    void startIdentification();
    void update();

    bool isReady() const;
    bool hasError() const;

    std::string getVIN() const;
    std::string getSW() const;
    std::string getHW() const;
    std::string getError() const;

private:
    enum class Step
    {
        Idle,
        RequestVIN,
        WaitVIN,
        RequestSW,
        WaitSW,
        RequestHW,
        WaitHW,
        Done,
        Error
    };

    bool parseDidStringResponse(const std::vector<uint8_t>& resp,
                                uint16_t expectedDid,
                                std::string& out);
    void fail(const std::string& msg);

private:
    UDS_Core& core_;
    Step step_ = Step::Idle;

    std::string vin_;
    std::string sw_;
    std::string hw_;
    std::string error_;
};
