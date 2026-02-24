#pragma once

#include <cstdint>
#include <vector>
#include <chrono>
#include "core/CAN_FrameHandler.h"
#include "transport/ITransport_CAN.h"

class ISOTP : public CAN_FrameHandler
{
public:
    ISOTP(ITransport_CAN& can,
          uint32_t txId,
          uint32_t rxId);

    void handleFrame(uint32_t id,
                     const uint8_t* data,
                     uint8_t len) override;

    void update();   // wywoływane co 1 ms

    bool send(const std::vector<uint8_t>& payload);

    bool hasMessage() const;
    std::vector<uint8_t> receive();

private:
    enum class RxState
    {
        Idle,
        Receiving
    };

    enum class TxState
    {
        Idle,
        WaitingFC,
        SendingCF
    };

    void sendFlowControl();
    void sendSingleFrame(const std::vector<uint8_t>& payload);
    void sendFirstFrame(const std::vector<uint8_t>& payload);
    void sendConsecutiveFrame();

private:
    ITransport_CAN& can_;
    uint32_t txId_;
    uint32_t rxId_;

    // RX
    RxState rxState_ = RxState::Idle;
    std::vector<uint8_t> rxBuffer_;
    uint16_t rxExpectedLength_ = 0;
    uint8_t rxNextSN_ = 1;

    // TX
    TxState txState_ = TxState::Idle;
    std::vector<uint8_t> txBuffer_;
    uint16_t txIndex_ = 0;
    uint8_t txSN_ = 1;
    uint8_t blockSize_ = 0;
    uint8_t stMin_ = 0;
    uint8_t blockCounter_ = 0;

    std::chrono::steady_clock::time_point lastTxTime_;
    std::chrono::steady_clock::time_point lastRxTime_;

    bool messageReady_ = false;
};