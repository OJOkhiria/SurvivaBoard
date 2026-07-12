#include "mmc5983ma.hpp"

#include "hal_i2c.hpp"

namespace
{
constexpr uint8_t PRODUCT_ID_REGISTER = 0x2F;
}

esp_err_t MMC5983MA::begin()
{
    uint8_t product_id = 0;
    const esp_err_t result = HAL::I2C::readRegister(
        ADDRESS, PRODUCT_ID_REGISTER, &product_id, sizeof(product_id));
    present_ = result == ESP_OK && product_id == PRODUCT_ID;
    return present_ ? ESP_OK : ESP_ERR_NOT_FOUND;
}

bool MMC5983MA::isPresent() const
{
    return present_;
}
