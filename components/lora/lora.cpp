#include "lora.hpp"

#include "config.hpp"
#include "driver/gpio.h"
#include "hal_spi.hpp"
#include "pins.hpp"

esp_err_t LoRa::begin()
{
    const gpio_config_t control_pins = {
        .pin_bit_mask = (1ULL << Pins::LORA_RESET) |
                        (1ULL << Pins::LORA_TXEN) |
                        (1ULL << Pins::LORA_RXEN),
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
        .pin_bit_mask = (1ULL << Pins::LORA_BUSY) |
                        (1ULL << Pins::LORA_DIO1),
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

    gpio_set_level(Pins::LORA_RESET, 1);
    setReceiveMode();
    return HAL::SPI::addDevice(Settings::SPI_SPEED_LORA, 4, Pins::LORA_CS,
                               &device_);
}

void LoRa::setReceiveMode()
{
    gpio_set_level(Pins::LORA_TXEN, 0);
    gpio_set_level(Pins::LORA_RXEN, 1);
}

void LoRa::setTransmitMode()
{
    gpio_set_level(Pins::LORA_RXEN, 0);
    gpio_set_level(Pins::LORA_TXEN, 1);
}
