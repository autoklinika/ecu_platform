#include <iostream>
#include "transport/Transport_CAN_Linux.h"

int main()
{
    std::cout << "Starting raw CAN listener...\n";

    Transport_CAN_Linux can("can0");

    while (true)
    {
        uint32_t id;
        uint8_t data[8];
        uint8_t len;

        if (can.receiveFrame(id, data, len))
        {
            std::cout << "RX ID: 0x"
                      << std::hex << id
                      << " LEN: "
                      << std::dec << (int)len
                      << std::endl;
        }
    }

    return 0;
}