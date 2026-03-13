#pragma once

#include "uds/UDS_Core.h"

#include <cstdint>
#include <string>
#include <vector>

class SAC_DTC_Module
{
public:
    struct DTCRecord
    {
        uint32_t code = 0;
        uint8_t status = 0;
    };

    explicit SAC_DTC_Module(UDS_Core& core);

    void startRead();
    void resetToIdle();
    void update();

    bool isDone() const;
    bool hasError() const;
    bool isBusy() const;
    bool isIdle() const;

    std::vector<DTCRecord> getDTCs() const;
    std::string getError() const;

private:
    enum class State
    {
        Idle,
        RequestSession,
        WaitSession,
        RequestRead,
        WaitRead,
        Done,
        Error
    };

    bool parseDTCResponse(const std::vector<uint8_t>& resp);

private:
    UDS_Core& uds_;
    State state_ = State::Idle;
    bool sessionActive_ = false;

    std::vector<DTCRecord> dtcs_;
    std::string error_;
};
