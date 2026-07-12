#pragma once

#include <cstddef>
#include <cstdint>

#include "driver/i2c.h"
#include "esp_err.h"

namespace HAL::I2C
{
esp_err_t init();
esp_err_t readRegister(uint8_t address, uint8_t reg, uint8_t* data,
                       size_t length, int timeout_ms = 100);
esp_err_t writeRegister(uint8_t address, uint8_t reg,
                        const uint8_t* data, size_t length,
                        int timeout_ms = 100);
}  // namespace HAL::I2C
