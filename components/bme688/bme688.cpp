#include "bme688.hpp"

#include "hal_i2c.hpp"

namespace
{
constexpr uint8_t CHIP_ID_REGISTER = 0xD0;
}

esp_err_t BME688::begin()
{
    uint8_t chip_id = 0;
    const esp_err_t result = HAL::I2C::readRegister(
        ADDRESS, CHIP_ID_REGISTER, &chip_id, sizeof(chip_id));
    present_ = result == ESP_OK && chip_id == CHIP_ID;
    return present_ ? ESP_OK : ESP_ERR_NOT_FOUND;
}

bool BME688::isPresent() const
{
    return present_;
}
