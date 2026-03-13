#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <optional>
#include <chrono>

class ISOTP;

class UDS_Client
{
public:
    explicit UDS_Client(ISOTP& isotp);

    bool startExtendedSession();
    std::optional<std::string> readVIN();

    std::optional<std::vector<uint8_t>> readDTC();
    bool clearDTC();

    void testerPresent();

private:
    std::optional<std::vector<uint8_t>>
    sendRequest(const std::vector<uint8_t>& request,
                std::chrono::milliseconds timeout =
                    std::chrono::milliseconds(2000));

private:
    ISOTP& isotp_;
};
