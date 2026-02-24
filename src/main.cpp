#include "core/Frame.h"
#include "core/FrameQueue.h"
#include "core/CAN_Dispatcher.h"
#include "core/CAN_FrameHandler.h"
#include "transport/Transport_CAN_Linux.h"
#include "isotp/ISOTP.h"
#include "uds/UDS_Core.h"

#include <thread>
#include <iostream>
#include <cstring>
#include <chrono>
#include <vector>
#include <atomic>

using namespace std::chrono;

// =======================
// CAN RX THREAD
// =======================
void canRxThread(ITransport_CAN& can,
                 FrameQueue& queue,
                 std::atomic<bool>& running)
{
    while(running)
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
                    ISOTP& isotp,
                    UDS_Core& udsCore,
                    std::atomic<bool>& running)
{
    while(running)
    {
        Frame f;

        while(queue.tryPop(f))
        {
            dispatcher.dispatch(f.id, f.data, f.len);
        }

        isotp.update();
        udsCore.update();

        std::this_thread::sleep_for(milliseconds(1));
    }
}

// =======================
// MAIN
// =======================
int main()
{
    const std::string iface = "can0";
    const int bitrate = 250000;   // ręcznie ustawione

    Transport_CAN_Linux can;

    if(!can.open(iface, bitrate))
    {
        std::cerr << "CAN open failed\n";
        return 1;
    }

    std::atomic<bool> running{true};

    FrameQueue queue;
    CAN_Dispatcher dispatcher;

    // ===== 29-bit UDS addressing =====
    uint32_t testerToEcu = 0x18DA30F9;
    uint32_t ecuToTester = 0x18DAF930;

    ISOTP isotp(can, testerToEcu, ecuToTester);
    UDS_Core udsCore(isotp);

    dispatcher.registerHandler(&isotp);

    // ===== Start threads =====
    std::thread rxThread(canRxThread,
                         std::ref(can),
                         std::ref(queue),
                         std::ref(running));

    std::thread protoThread(protocolThread,
                            std::ref(queue),
                            std::ref(dispatcher),
                            std::ref(isotp),
                            std::ref(udsCore),
                            std::ref(running));

    std::cout << "System running...\n";

    std::this_thread::sleep_for(seconds(1));

    // ==========================
    // 1️⃣ Extended Session
    // ==========================

    if(!udsCore.request({0x10, 0x03}))
    {
        std::cout << "Session request send failed\n";
        running = false;
        return 1;
    }

    while(udsCore.getState() == UDS_Core::State::WaitingResponse)
        std::this_thread::sleep_for(milliseconds(5));

    if(udsCore.getState() == UDS_Core::State::Done)
    {
        auto resp = udsCore.getResponse();

        if(resp.size() >= 2 && resp[0] == 0x50)
        {
            std::cout << "Extended session started\n";
            udsCore.setSessionActive(true);
        }
        else
        {
            std::cout << "Session negative response\n";
            running = false;
        }
    }
    else
    {
        std::cout << "Session timeout/error\n";
        running = false;
    }

    // ==========================
    // 2️⃣ Read VIN
    // ==========================

    if(running)
    {
        if(!udsCore.request({0x22, 0xF1, 0x90}))
        {
            std::cout << "VIN request send failed\n";
            running = false;
        }

        while(running &&
              udsCore.getState() == UDS_Core::State::WaitingResponse)
        {
            std::this_thread::sleep_for(milliseconds(5));
        }

        if(udsCore.getState() == UDS_Core::State::Done)
        {
            auto resp = udsCore.getResponse();

            if(resp.size() > 3 && resp[0] == 0x62)
            {
                std::string vin(resp.begin() + 3, resp.end());
                std::cout << "VIN: " << vin << "\n";
            }
            else
            {
                std::cout << "VIN invalid response\n";
            }
        }
        else
        {
            std::cout << "VIN timeout/error\n";
        }
    }

    std::cout << "Press ENTER to stop\n";
    std::cin.get();

    // ===== Clean shutdown =====
    running = false;
    queue.stop();
    can.close();

    rxThread.join();
    protoThread.join();

    return 0;
}