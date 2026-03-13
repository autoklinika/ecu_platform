#include "SAC_Runtime_Module.h"

using namespace std::chrono;

SAC_Runtime_Module::SAC_Runtime_Module(UDS_Core& core)
    : uds_(core)
{
    lastVoltageRequest_ = steady_clock::now();
}

void SAC_Runtime_Module::begin()
{
    pressure1Bar_.reset();
    pressure2Bar_.reset();
    voltagePermanent_.reset();
    voltageIgnition_.reset();
    udsState_ = UDSState::Idle;
    lastVoltageRequest_ = steady_clock::now();
}

uint32_t SAC_Runtime_Module::decodePgn(uint32_t id)
{
    const uint8_t pf = (id >> 16) & 0xFF;
    const uint8_t dp = (id >> 24) & 0x01;
    const uint8_t ps = (id >> 8) & 0xFF;

    if (pf < 240)
        return (static_cast<uint32_t>(dp) << 16) |
               (static_cast<uint32_t>(pf) << 8);

    return (static_cast<uint32_t>(dp) << 16) |
           (static_cast<uint32_t>(pf) << 8) |
           ps;
}

void SAC_Runtime_Module::handleFrame(uint32_t id,
                                     const uint8_t* data,
                                     uint8_t len)
{
    if (len < 4)
        return;

    // Ignoruj UDS (PF 0xDA), interesuje nas J1939 broadcast
    const uint8_t pf = (id >> 16) & 0xFF;
    if (pf == 0xDA)
        return;

    const uint32_t pgn = decodePgn(id);

    // Arduino: PGN 65198, data[2], data[3], skala 0.08 bar
    if (pgn == 65198 && len >= 4)
    {
        pressure1Bar_ = static_cast<float>(data[2]) * 0.08f;
        pressure2Bar_ = static_cast<float>(data[3]) * 0.08f;
    }
}

void SAC_Runtime_Module::update()
{
    const auto now = steady_clock::now();

    switch (udsState_)
    {
    case UDSState::Idle:
        if (!uds_.isIdle())
            return;

        if (duration_cast<milliseconds>(now - lastVoltageRequest_) <
            VOLTAGE_REQUEST_PERIOD)
            return;

        // Arduino runtime: DID 0xFE96
        if (uds_.request({0x22, 0xFE, 0x96}))
        {
            udsState_ = UDSState::WaitingVoltage;
            lastVoltageRequest_ = now;
        }
        break;

    case UDSState::WaitingVoltage:
        if (uds_.getState() == UDS_Core::State::Done)
        {
            auto resp = uds_.getResponse();

            // Oczekujemy: 62 FE 96 + 8 bajtów payloadu
            // Arduino po zdjęciu DID brał bytes [4..7]
            // czyli tutaj: resp[7..10]
            if (resp.size() >= 11 &&
                resp[0] == 0x62 &&
                resp[1] == 0xFE &&
                resp[2] == 0x96)
            {
                const uint16_t rawPermanent =
                    (static_cast<uint16_t>(resp[7]) << 8) |
                    static_cast<uint16_t>(resp[8]);

                const uint16_t rawIgnition =
                    (static_cast<uint16_t>(resp[9]) << 8) |
                    static_cast<uint16_t>(resp[10]);

                voltagePermanent_ = rawPermanent / 10.0f;
                voltageIgnition_ = rawIgnition / 10.0f;
            }

            uds_.reset();
            udsState_ = UDSState::Idle;
        }
        else if (uds_.getState() == UDS_Core::State::Error ||
                 uds_.getState() == UDS_Core::State::Timeout)
        {
            uds_.reset();
            udsState_ = UDSState::Idle;
        }
        break;
    }
}
