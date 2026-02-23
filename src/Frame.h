#pragma once
#include <cstdint>

struct Frame
{
    uint32_t id;
    uint8_t  data[8];
    uint8_t  len;
};