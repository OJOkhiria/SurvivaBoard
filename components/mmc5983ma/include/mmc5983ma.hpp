#pragma once

#include <cstdint>

#include "esp_err.h"

class MMC5983MA
{
public:
    static constexpr uint8_t ADDRESS = 0x30;
    static constexpr uint8_t PRODUCT_ID = 0x30;

    esp_err_t begin();
    bool isPresent() const;

private:
    bool present_ = false;
};
