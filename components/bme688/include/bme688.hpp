#pragma once

#include <cstdint>

#include "esp_err.h"

class BME688
{
public:
    static constexpr uint8_t ADDRESS = 0x76;
    static constexpr uint8_t CHIP_ID = 0x61;

    esp_err_t begin();
    bool isPresent() const;

private:
    bool present_ = false;
};
