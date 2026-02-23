#include "core/Frame.h"
#include "core/FrameQueue.h"
#include "core/CAN_Dispatcher.h"
#include "core/CAN_FrameHandler.h"
#include "transport/Transport_CAN_Linux.h"
#include "isotp/ISOTP.h"
#include "uds/UDS_Client.h"

#include <thread>
#include <iostream>
#include <cstring>
#include <chrono>
#include <vector>

using namespace std::chrono;

// =======================
// CAN RX THREAD
// =======================
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

// =======================
// PROTOCOL THREAD
// =======================
void protocolThread(FrameQueue &queue,
                    CAN_Dispatcher &dispatcher,
                    ISOTP& isotp)
{
    while(true)
    {
        Frame f;

        if(!queue.pop(f))
            break;

        // 1️⃣ Dispatcher → przekazuje do ISOTP
        dispatcher.dispatch(f.id, f.data, f.len);

        // 2️⃣ ISO-TP tick
        isotp.update();
    }
}

// =======================
// MAIN
// =======================
int main()
{
    Transport_CAN_Linux can("can0");

    if(!can.isValid())
    {
        std::cerr << "CAN init failed\n";
        return 1;
    }

    FrameQueue queue;
    CAN_Dispatcher dispatcher;

    // ===== 29-bit UDS addressing =====
    // Tester SA = 0xF9
    // ECU SA    = 0x30
    uint32_t testerToEcu = 0x18DA30F9;
    uint32_t ecuToTester = 0x18DAF930;

    ISOTP isotp(can, testerToEcu, ecuToTester);
    UDS_Client uds(isotp);

    dispatcher.registerHandler(&isotp);

    std::thread rxThread(canRxThread,
                         std::ref(can),
                         std::ref(queue));

    std::thread protoThread(protocolThread,
                            std::ref(queue),
                            std::ref(dispatcher),
                            std::ref(isotp));

    std::cout << "System running...\n";

    // ==========================
    // TEST SEKWENCJA UDS
    // ==========================

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 1️⃣ Extended Session
    if(uds.startExtendedSession())
    {
        std::cout << "Extended session started\n";
    }
    else
    {
        std::cout << "Session start failed\n";
        return 1;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    // 2️⃣ Read VIN
    auto vin = uds.readVIN();

    if(vin)
    {
        std::cout << "VIN: " << *vin << std::endl;
    }
    else
    {
        std::cout << "VIN read failed\n";
    }

    std::cout << "Press ENTER to stop\n";
    std::cin.get();

    queue.stop();
    can.closeSocket();

    rxThread.join();
    protoThread.join();

    return 0;
}