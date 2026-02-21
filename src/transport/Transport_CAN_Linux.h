#pragma once

#include <cstdint>

class Transport_CAN_Linux
{
public:
    explicit Transport_CAN_Linux(const char* interface = "can0");
    ~Transport_CAN_Linux();

    bool receiveFrame(uint32_t& id,
                      uint8_t* data,
                      uint8_t& len);

    bool sendFrame(uint32_t id,
                   const uint8_t* data,
                   uint8_t len,
                   bool extended = true);

    bool isValid() const;

private:
    int socket_fd;
};