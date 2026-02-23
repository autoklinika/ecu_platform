#include "FrameQueue.h"
#include "Transport_CAN_Linux.h"
#include "Frame.h"
#include "CAN_Dispatcher.h"
#include "CAN_FrameHandler.h"

#include <thread>
#include <iostream>
#include <atomic>
#include <cstring>
#include <chrono>

using namespace std::chrono;

class DebugHandler : public CAN_FrameHandler
{
public:
    void handleFrame(uint32_t id,
                     const uint8_t* data,
                     uint8_t len) override
    {
        std::cout << "DISPATCH 0x"
                  << std::hex << id
                  << std::dec << std::endl;
    }
};

void canRxThread(ITransport_CAN& can,
                 FrameQueue& queue)
{
    while(true)
    {
        uint32_t id;
        uint8_t data[8];
        uint8_t len;

        if(can.receiveFrame(id, data, len))
        {
            Frame f{};
            f.id = id;
            f.len = len;
            std::memcpy(f.data, data, len);

            queue.push(f);
        }
    }
}

void protocolThread(FrameQueue &queue,
                    CAN_Dispatcher &dispatcher)
{
    using clock = std::chrono::steady_clock;
    auto lastTick = clock::now();

    while (true)
    {
        Frame f;

        if (!queue.pop(f))
            break;

        dispatcher.dispatch(f.id, f.data, f.len);

        auto now = clock::now();
        if (now - lastTick >= std::chrono::milliseconds(1))
        {
            lastTick = now;
            // tutaj później będzie isotp.update();
        }
    }
}

int main()
{
    Transport_CAN_Linux can("can0");

    if (!can.isValid())
    {
        std::cerr << "CAN init failed\n";
        return 1;
    }

    FrameQueue queue;
    CAN_Dispatcher dispatcher;
    DebugHandler debug;

    dispatcher.registerHandler(&debug);

    std::thread rxThread(canRxThread,
                         std::ref(can),
                         std::ref(queue));

    std::thread protoThread(protocolThread,
                            std::ref(queue),
                            std::ref(dispatcher));

    std::cout << "System running...\n";
    std::cout << "Press ENTER to stop\n";
    std::cin.get();

    queue.stop();
    can.closeSocket();

    rxThread.join();
    protoThread.join();

    return 0;
}