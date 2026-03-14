#pragma once

#include "uds/UDS_Core.h"
#include <cstdint>
#include <string>
#include <vector>

class SAC_DTC_Module
{
public:
    struct DTCItem
    {
        uint32_t code = 0;
        uint8_t status = 0;
    };

    explicit SAC_DTC_Module(UDS_Core& core);

    void startRead();
    void startClear();
    void update();

    bool isBusy() const;
    bool isDone() const;
    bool hasError() const;

    const std::vector<DTCItem>& getDTCs() const;
    std::string getError() const;

private:
    enum class Op
    {
        None,
        Read,
        Clear
    };

    enum class Step
    {
        Idle,
        RequestSession,
        WaitSession,
        RequestRead,
        WaitRead,
        RequestClear,
        WaitClear,
        Done,
        Error
    };

    void resetResult();
    void fail(const std::string& msg);
    void parseReadResponse(const std::vector<uint8_t>& resp);

private:
    UDS_Core& core_;
    Op op_ = Op::None;
    Step step_ = Step::Idle;

    std::vector<DTCItem> dtcs_;
    std::string error_;
};
