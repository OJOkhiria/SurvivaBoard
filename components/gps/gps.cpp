#include "gps.hpp"

#include "driver/gpio.h"
#include "hal_uart.hpp"
#include "pins.hpp"

esp_err_t GPS::begin()
{
    const gpio_config_t timepulse_config = {
        .pin_bit_mask = 1ULL << board::pins::gps_timepulse,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t result = gpio_config(&timepulse_config);
    if (result != ESP_OK)
    {
        return result;
    }

    const gpio_config_t extint_config = {
        .pin_bit_mask = 1ULL << board::pins::gps_extint,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    result = gpio_config(&extint_config);
    if (result != ESP_OK)
    {
        return result;
    }
    gpio_set_level(board::pins::gps_extint, 0);

    return HAL::UART::initGps();
}

int GPS::read(uint8_t* data, const size_t length, const uint32_t timeout_ms)
{
    return HAL::UART::readGps(data, length, timeout_ms);
}

int GPS::write(const uint8_t* data, const size_t length)
{
    return HAL::UART::writeGps(data, length);
}
