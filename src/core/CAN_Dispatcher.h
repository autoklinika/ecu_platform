#pragma once

#include <vector>
#include <cstdint>
#include "CAN_FrameHandler.h"

class CAN_Dispatcher
{
public:
    void registerHandler(CAN_FrameHandler* handler);

    void dispatch(uint32_t id,
                  const uint8_t* data,
                  uint8_t len);

private:
    std::vector<CAN_FrameHandler*> handlers;
};