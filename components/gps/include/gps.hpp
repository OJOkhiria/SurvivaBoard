#pragma once

#include <cstddef>
#include <cstdint>

#include "esp_err.h"

class GPS
{
public:
    esp_err_t begin();
    int read(uint8_t* data, size_t length, uint32_t timeout_ms = 100);
    int write(const uint8_t* data, size_t length);
};
