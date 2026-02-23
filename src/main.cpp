#include "FrameQueue.h"
#include "Transport_CAN_Linux.h"

#include <thread>
#include <iostream>
#include <atomic>
#include <cstring>
#include <chrono>

using namespace std::chrono;

void canRxThread(ITransport_CAN& can,
                 FrameQueue& queue)
{
    while(queue.isRunning())
    {
        uint32_t id;
        uint8_t data[8];
        uint8_t len;

        if(can.receiveFrame(id, data, len))
        {
            Frame f {};
            f.id  = id;
            f.len = len;
            std::memcpy(f.data, data, len);

            queue.push(f);
        }
    }
}

void protocolThread(FrameQueue& queue)
{
    auto lastTick = steady_clock::now();

    while(queue.isRunning())
    {
        Frame f {};

        // Czekaj max 1 ms
        bool ok = queue.waitAndPop(f, milliseconds(1));

        if(!queue.isRunning())
            break;

        // Jeśli przyszła ramka
        if(f.len > 0)
        {
            std::cout << "RX ID: 0x"
                      << std::hex << f.id
                      << std::dec << std::endl;

            // TODO:
            // dispatcher.dispatch(...)
        }

        // 1ms tick dla ISO-TP / UDS
        auto now = steady_clock::now();
        if(now - lastTick >= milliseconds(1))
        {
            lastTick = now;

            // TODO:
            // isotp.update();
            // uds.update();
        }
    }
}

int main()
{
    Transport_CAN_Linux can("can0");

    if(!can.isValid())
    {
        std::cerr << "CAN init failed\n";
        return 1;
    }

    FrameQueue queue;

    std::thread rxThread(canRxThread,
                         std::ref(can),
                         std::ref(queue));

    std::thread protoThread(protocolThread,
                            std::ref(queue));

    std::cout << "System running...\n";
    std::cout << "Press ENTER to stop\n";
    std::cin.get();

    queue.stop();
    can.closeSocket();

    rxThread.join();
    protoThread.join();

    return 0;
}