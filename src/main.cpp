#include "core/Frame.h"
#include "core/FrameQueue.h"
#include "core/CAN_Dispatcher.h"
#include "core/CAN_FrameHandler.h"
#include "transport/Transport_CAN_Linux.h"
#include "isotp/ISOTP.h"

#include <thread>
#include <iostream>
#include <cstring>
#include <chrono>
#include <vector>

using namespace std::chrono;

// =======================
// Debug handler (opcjonalny)
// =======================
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
    auto lastTick = steady_clock::now();

    while (true)
    {
        Frame f;

        if (!queue.pop(f))
            break;

        dispatcher.dispatch(f.id, f.data, f.len);

        // ISO-TP state machine tick
        isotp.update();

        // Sprawdzenie czy złożona wiadomość
        if(isotp.hasMessage())
        {
            auto response = isotp.receive();

            std::cout << "UDS Response: ";
            for(auto b : response)
                std::cout << std::hex << (int)b << " ";
            std::cout << std::dec << std::endl;

            // Positive Response Session
            if(response.size() >= 2 &&
               response[0] == 0x50)
            {
                std::cout << "Session started\n";
            }

            // Positive Response VIN
            if(response.size() > 3 &&
               response[0] == 0x62 &&
               response[1] == 0xF1 &&
               response[2] == 0x90)
            {
                std::string vin(response.begin()+3, response.end());
                std::cout << "VIN: " << vin << std::endl;
            }

            // Negative response
            if(response.size() >= 3 &&
               response[0] == 0x7F)
            {
                std::cout << "Negative Response NRC: 0x"
                          << std::hex << (int)response[2]
                          << std::dec << std::endl;
            }
        }

        auto now = steady_clock::now();
        if (now - lastTick >= milliseconds(1))
        {
            lastTick = now;
        }
    }
}

// =======================
// MAIN
// =======================
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

    // ===== 29-bit UDS addressing =====
    // Tester SA = 0xF9
    // ECU SA    = 0x30
    uint32_t testerToEcu = 0x18DA30F9;
    uint32_t ecuToTester = 0x18DAF930;

    ISOTP isotp(can, testerToEcu, ecuToTester);

    dispatcher.registerHandler(&isotp);
    //dispatcher.registerHandler(new DebugHandler()); // opcjonalnie

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

    // 1️⃣ Diagnostic Session Control (Extended)
    std::vector<uint8_t> sessionRequest = {0x10, 0x03};

    if(isotp.send(sessionRequest))
        std::cout << "Session request sent\n";
    else
        std::cout << "ISO-TP busy (session)\n";

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 2️⃣ Read VIN
    std::vector<uint8_t> vinRequest = {0x22, 0xF1, 0x90};

    if(isotp.send(vinRequest))
        std::cout << "VIN request sent\n";
    else
        std::cout << "ISO-TP busy (VIN)\n";

    std::cout << "Press ENTER to stop\n";
    std::cin.get();

    queue.stop();
    can.closeSocket();

    rxThread.join();
    protoThread.join();

    return 0;
}