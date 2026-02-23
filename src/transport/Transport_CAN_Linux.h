#pragma once

#include "ITransport_CAN.h"

#include <string>

class Transport_CAN_Linux : public ITransport_CAN
{
public:
    explicit Transport_CAN_Linux(const std::string& ifname);
    ~Transport_CAN_Linux();

    bool sendFrame(uint32_t id,
                   const uint8_t* data,
                   uint8_t len) override;

    bool receiveFrame(uint32_t& id,
                      uint8_t* data,
                      uint8_t& len) override;

    bool isValid() const override;

    void closeSocket();

private:
    int socket_ = -1;
};