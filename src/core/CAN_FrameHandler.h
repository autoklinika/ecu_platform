#pragma once

#include <cstdint>

class CAN_FrameHandler
{
public:
    virtual ~CAN_FrameHandler() = default;

    virtual void handleFrame(uint32_t id,
                             const uint8_t* data,
                             uint8_t len) = 0;
};