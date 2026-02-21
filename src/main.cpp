#include <iostream>
#include <thread>
#include <chrono>
#include "transport/Transport_CAN_Linux.h"

int main()
{
    Transport_CAN_Linux can("can0");

    if (!can.isValid())
    {
        std::cerr << "CAN init failed\n";
        return 1;
    }

    std::cout << "CAN listener started\n";

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

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}