#include "ISOTP.h"
#include <cstring>


using namespace std::chrono;

ISOTP::ISOTP(ITransport_CAN& can,
             uint32_t txId,
             uint32_t rxId)
    : can_(can),
      txId_(txId),
      rxId_(rxId)
{
}

void ISOTP::handleFrame(uint32_t id,
                        const uint8_t* data,
                        uint8_t len)
{
    if(id != rxId_ || len < 1)
        return;

    uint8_t pciType = data[0] >> 4;

    if (pciType == 0x0) // Single Frame
    {
        uint8_t size = data[0] & 0x0F;
        rxBuffer_.assign(&data[1], &data[1] + size);
        messageReady_ = true;
    }
    else if (pciType == 0x1) // First Frame
    {
        messageReady_ = false; // 🔥 KLUCZOWE

        rxExpectedLength_ = ((data[0] & 0x0F) << 8) | data[1];

        rxBuffer_.clear();
        rxBuffer_.insert(rxBuffer_.end(), &data[2], &data[8]);

        rxState_ = RxState::Receiving;
        rxNextSN_ = 1;

        sendFlowControl();
    }
    else if (pciType == 0x2 && rxState_ == RxState::Receiving) // CF
    {
        uint8_t sn = data[0] & 0x0F;
        if(sn != rxNextSN_)
        {
            rxState_ = RxState::Idle;
            return;
        }

        rxNextSN_ = (rxNextSN_ + 1) & 0x0F;

        size_t remaining = rxExpectedLength_ - rxBuffer_.size();
        size_t copyLen = remaining > 7 ? 7 : remaining;

        rxBuffer_.insert(rxBuffer_.end(), &data[1], &data[1] + copyLen);

        if(rxBuffer_.size() == rxExpectedLength_)
        {
            rxState_ = RxState::Idle;
            messageReady_ = true;
        }
    }
    else if(pciType == 0x3) // Flow Control
    {
        blockSize_ = data[1];
        stMin_ = data[2];
        txState_ = TxState::SendingCF;
        blockCounter_ = 0;
        lastTxTime_ = std::chrono::steady_clock::now();
    }

    lastRxTime_ = std::chrono::steady_clock::now();
}

void ISOTP::update()
{
    if(txState_ == TxState::SendingCF)
    {
        auto now = std::chrono::steady_clock::now();
        if(duration_cast<milliseconds>(now - lastTxTime_).count() >= stMin_)
        {
            sendConsecutiveFrame();
            lastTxTime_ = now;
        }
    }
}

bool ISOTP::send(const std::vector<uint8_t>& payload)
{
    if(txState_ != TxState::Idle)
        return false;

    if(payload.size() <= 7)
        sendSingleFrame(payload);
    else
        sendFirstFrame(payload);

    return true;
}

void ISOTP::sendSingleFrame(const std::vector<uint8_t>& payload)
{
    uint8_t frame[8] = {};
    frame[0] = payload.size();
    std::memcpy(&frame[1], payload.data(), payload.size());
    can_.sendFrame(txId_, frame, 8);
}

void ISOTP::sendFirstFrame(const std::vector<uint8_t>& payload)
{
    txBuffer_ = payload;
    txIndex_ = 6;
    txSN_ = 1;

    uint8_t frame[8] = {};
    frame[0] = 0x10 | ((payload.size() >> 8) & 0x0F);
    frame[1] = payload.size() & 0xFF;

    std::memcpy(&frame[2], payload.data(), 6);

    can_.sendFrame(txId_, frame, 8);
    txState_ = TxState::WaitingFC;
}

void ISOTP::sendConsecutiveFrame()
{
    if(txIndex_ >= txBuffer_.size())
    {
        txState_ = TxState::Idle;
        return;
    }

    uint8_t frame[8] = {};
    frame[0] = 0x20 | (txSN_ & 0x0F);

    size_t remaining = txBuffer_.size() - txIndex_;
    size_t copyLen = remaining > 7 ? 7 : remaining;

    std::memcpy(&frame[1], &txBuffer_[txIndex_], copyLen);

    can_.sendFrame(txId_, frame, 8);

    txIndex_ += copyLen;
    txSN_ = (txSN_ + 1) & 0x0F;
}

void ISOTP::sendFlowControl()
{
    uint8_t frame[8] = {};
    frame[0] = 0x30; // FC, CTS
    frame[1] = 0;    // Block size
    frame[2] = 0;    // STmin

    can_.sendFrame(txId_, frame, 8);
}

bool ISOTP::hasMessage() const
{
    return messageReady_;
}

std::vector<uint8_t> ISOTP::receive()
{
    messageReady_ = false;
    return rxBuffer_;
}