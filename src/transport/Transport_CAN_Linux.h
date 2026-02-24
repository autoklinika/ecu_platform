#pragma once

#include <string>
#include <cstdint>
#include "transport/ITransport_CAN.h"

class Transport_CAN_Linux : public ITransport_CAN
{
public:
    Transport_CAN_Linux();
    ~Transport_CAN_Linux();

    bool open(const std::string& iface, int bitrate);
    void close();

    // === ITransport_CAN interface ===
    bool sendFrame(uint32_t id,
                   const uint8_t* data,
                   uint8_t len) override;

    bool receiveFrame(uint32_t& id,
                      uint8_t* data,
                      uint8_t& len) override;

    bool isValid() const override;

private:
    int socketFd = -1;
    bool configureBitrate(const std::string& iface, int bitrate);
};