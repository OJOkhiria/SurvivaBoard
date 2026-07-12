#pragma once

#include <cstddef>
#include <cstdint>

#include "esp_err.h"

namespace HAL::UART
{
esp_err_t initGps();
int readGps(uint8_t* data, size_t length, uint32_t timeout_ms);
int writeGps(const uint8_t* data, size_t length);
}  // namespace HAL::UART
