#pragma once

#include "core/CAN_FrameHandler.h"
#include "uds/UDS_Core.h"

#include <chrono>
#include <cstdint>
#include <optional>

class SAC_Runtime_Module : public CAN_FrameHandler
{
public:
    explicit SAC_Runtime_Module(UDS_Core& core);

    void begin();
    void update();

    void handleFrame(uint32_t id,
                     const uint8_t* data,
                     uint8_t len) override;

    std::optional<float> pressure1Bar() const { return pressure1Bar_; }
    std::optional<float> pressure2Bar() const { return pressure2Bar_; }
    std::optional<float> voltagePermanent() const { return voltagePermanent_; }
    std::optional<float> voltageIgnition() const { return voltageIgnition_; }

private:
    enum class UDSState
    {
        Idle,
        WaitingVoltage
    };

    static uint32_t decodePgn(uint32_t id);

private:
    UDS_Core& uds_;
    UDSState udsState_ = UDSState::Idle;

    std::optional<float> pressure1Bar_;
    std::optional<float> pressure2Bar_;
    std::optional<float> voltagePermanent_;
    std::optional<float> voltageIgnition_;

    std::chrono::steady_clock::time_point lastVoltageRequest_;
    static constexpr std::chrono::milliseconds VOLTAGE_REQUEST_PERIOD{300};
};
