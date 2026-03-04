#include "UDS_Client.h"
#include "isotp/ISOTP.h"

#include <thread>

using namespace std::chrono;

UDS_Client::UDS_Client(ISOTP& isotp)
    : isotp_(isotp)
{
}

std::optional<std::vector<uint8_t>>
UDS_Client::sendRequest(const std::vector<uint8_t>& request,
                        milliseconds timeout)
{
    if(!isotp_.send(request))
        return std::nullopt;

    auto start = steady_clock::now();

    while(steady_clock::now() - start < timeout)
    {
        if(isotp_.hasMessage())
        {
            auto resp = isotp_.receive();

            // Negative response
            if(resp.size() >= 3 && resp[0] == 0x7F)
            {
                if(resp[2] == 0x78) // Response pending
                    continue;

                return std::nullopt;
            }

            return resp;
        }

        std::this_thread::sleep_for(milliseconds(5));
    }

    return std::nullopt;
}

bool UDS_Client::startExtendedSession()
{
    std::vector<uint8_t> req = {0x10, 0x03};

    auto resp = sendRequest(req);
    if(!resp)
        return false;

    return resp->size() >= 2 && resp->at(0) == 0x50;
}

std::optional<std::string> UDS_Client::readVIN()
{
    std::vector<uint8_t> req = {0x22, 0xF1, 0x90};

    auto resp = sendRequest(req);
    if(!resp)
        return std::nullopt;

    if(resp->size() < 3)
        return std::nullopt;

    if((*resp)[0] != 0x62)
        return std::nullopt;

    std::string vin(resp->begin() + 3, resp->end());
    return vin;
}

std::optional<std::vector<uint8_t>> UDS_Client::readDTC()
{
    std::vector<uint8_t> req = {0x19, 0x02};

    auto resp = sendRequest(req);
    if(!resp)
        return std::nullopt;

    if(resp->size() < 2)
        return std::nullopt;

    if((*resp)[0] != 0x59)
        return std::nullopt;

    return *resp;
}

bool UDS_Client::clearDTC()
{
    std::vector<uint8_t> req = {0x14, 0xFF, 0xFF, 0xFF};

    auto resp = sendRequest(req);
    if(!resp)
        return false;

    return resp->size() >= 1 && resp->at(0) == 0x54;
}

void UDS_Client::testerPresent()
{
    std::vector<uint8_t> req = {0x3E, 0x00};
    isotp_.send(req);
}
