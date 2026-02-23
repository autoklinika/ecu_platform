#include "CAN_Dispatcher.h"

void CAN_Dispatcher::registerHandler(CAN_FrameHandler* handler)
{
    handlers.push_back(handler);
}

void CAN_Dispatcher::dispatch(uint32_t id,
                              const uint8_t* data,
                              uint8_t len)
{
    for (auto* h : handlers)
    {
        h->handleFrame(id, data, len);
    }
}