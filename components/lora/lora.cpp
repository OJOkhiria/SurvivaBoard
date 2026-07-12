#include "lora.hpp"

#include "config.hpp"
#include "driver/gpio.h"
#include "hal_spi.hpp"
#include "pins.hpp"

esp_err_t LoRa::begin()
{
    const gpio_config_t control_pins = {
        .pin_bit_mask = (1ULL << board::pins::lora_reset) |
                        (1ULL << board::pins::lora_txen) |
                        (1ULL << board::pins::lora_rxen),
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

    const gpio_config_t inputs = {
        .pin_bit_mask = (1ULL << board::pins::lora_busy) |
                        (1ULL << board::pins::lora_dio1),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    result = gpio_config(&inputs);
    if (result != ESP_OK)
    {
        return result;
    }

    gpio_set_level(board::pins::lora_reset, 1);
    setReceiveMode();
    return HAL::SPI::addDevice(Settings::SPI_SPEED_LORA, 4,
                               board::pins::lora_cs, &device_);
}

void LoRa::setReceiveMode()
{
    gpio_set_level(board::pins::lora_txen, 0);
    gpio_set_level(board::pins::lora_rxen, 1);
}

void LoRa::setTransmitMode()
{
    gpio_set_level(board::pins::lora_rxen, 0);
    gpio_set_level(board::pins::lora_txen, 1);
}
