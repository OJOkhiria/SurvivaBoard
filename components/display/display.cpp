#include "display.hpp"

#include "config.hpp"
#include "driver/gpio.h"
#include "hal_spi.hpp"
#include "pins.hpp"

esp_err_t Display::begin()
{
    const gpio_config_t control_pins = {
        .pin_bit_mask = (1ULL << Pins::TFT_DC) | (1ULL << Pins::TFT_RST) |
                        (1ULL << Pins::TFT_PWM),
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

    gpio_set_level(Pins::TFT_DC, 1);
    gpio_set_level(Pins::TFT_RST, 1);
    gpio_set_level(Pins::TFT_PWM, 0);

    return HAL::SPI::addDevice(Settings::SPI_SPEED_DISPLAY, 7, Pins::TFT_CS,
                               &device_);
}
