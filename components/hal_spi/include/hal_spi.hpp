#pragma once

#include <cstddef>
#include <cstdint>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"

namespace HAL::SPI
{
esp_err_t init();
esp_err_t addDevice(int clock_hz, int queue_size, gpio_num_t cs_pin,
                    spi_device_handle_t* device);
esp_err_t transmit(spi_device_handle_t device, const void* tx_data,
                   size_t length_bytes);
}  // namespace HAL::SPI
