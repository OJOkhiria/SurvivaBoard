#include "display.hpp"

#include "config.hpp"
#include "driver/gpio.h"
#include "hal_spi.hpp"
#include "pins.hpp"

esp_err_t Display::begin()
{
    const gpio_config_t control_pins = {
        .pin_bit_mask = (1ULL << board::pins::tft_dc) |
                        (1ULL << board::pins::tft_rst) |
                        (1ULL << board::pins::tft_pwm),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t result = gpio_config(&control_pins);
    if (result != ESP_OK)
    {
        return result;
    }

    gpio_set_level(board::pins::tft_dc, 1);
    gpio_set_level(board::pins::tft_rst, 1);
    // The CR2013-MI2120 module accepts a logic-high backlight-enable signal.
    gpio_set_level(board::pins::tft_pwm, 1);

    return HAL::SPI::addDevice(Settings::SPI_SPEED_DISPLAY, 7,
                               board::pins::tft_cs, &device_);
}
