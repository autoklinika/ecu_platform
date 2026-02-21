#pragma once

#include <cstdint>
#include "ITransport_CAN.h"

class Transport_CAN_Linux : public ITransport_CAN
{
public:
    explicit Transport_CAN_Linux(const char* interface = "can0");
    ~Transport_CAN_Linux() override;

    bool receiveFrame(uint32_t& id,
                      uint8_t* data,
                      uint8_t& len) override;

    bool sendFrame(uint32_t id,
                   const uint8_t* data,
                   uint8_t len,
                   bool extended = true) override;

    bool isValid() const override;

private:
    int socket_fd;
};