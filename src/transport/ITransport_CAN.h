#pragma once

#include <cstdint>

class ITransport_CAN
{
public:
    virtual ~ITransport_CAN() = default;

    virtual bool receiveFrame(uint32_t& id,
                              uint8_t* data,
                              uint8_t& len) = 0;

    virtual bool sendFrame(uint32_t id,
                           const uint8_t* data,
                           uint8_t len,
                           bool extended = true) = 0;

    virtual bool isValid() const = 0;
};