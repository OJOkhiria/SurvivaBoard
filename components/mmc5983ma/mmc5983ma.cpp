#include "mmc5983ma.hpp"

#include <cmath>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal_i2c.hpp"

namespace
{
constexpr uint8_t PRODUCT_ID_REGISTER = 0x2F;
constexpr uint8_t STATUS_REGISTER = 0x08;
constexpr uint8_t CONTROL_0_REGISTER = 0x09;
constexpr uint8_t XOUT_0_REGISTER = 0x00;
constexpr uint8_t MEASUREMENT_DONE = 0x01;
constexpr uint8_t TRIGGER_MEASUREMENT_WITH_AUTO_SR = 0x09;
constexpr float RAW_CENTER = 131072.0F;
constexpr float MILLIGAUSS_PER_LSB = 0.0625F;
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

esp_err_t MMC5983MA::read(MagneticField* field)
{
    if (field == nullptr || !present_)
    {
        return ESP_ERR_INVALID_STATE;
    }
    const uint8_t command = TRIGGER_MEASUREMENT_WITH_AUTO_SR;
    esp_err_t result = HAL::I2C::writeRegister(ADDRESS, CONTROL_0_REGISTER,
                                                &command, sizeof(command));
    if (result != ESP_OK)
    {
        return result;
    }

    uint8_t status = 0;
    for (int attempt = 0; attempt < 20; ++attempt)
    {
        vTaskDelay(pdMS_TO_TICKS(2));
        result = HAL::I2C::readRegister(ADDRESS, STATUS_REGISTER, &status,
                                        sizeof(status));
        if (result != ESP_OK)
        {
            return result;
        }
        if ((status & MEASUREMENT_DONE) != 0)
        {
            break;
        }
    }
    if ((status & MEASUREMENT_DONE) == 0)
    {
        return ESP_ERR_TIMEOUT;
    }

    uint8_t raw[7] = {};
    result = HAL::I2C::readRegister(ADDRESS, XOUT_0_REGISTER, raw, sizeof(raw));
    if (result != ESP_OK)
    {
        return result;
    }
    const uint32_t x = (static_cast<uint32_t>(raw[0]) << 10) |
                       (static_cast<uint32_t>(raw[1]) << 2) |
                       ((raw[6] >> 6) & 0x03);
    const uint32_t y = (static_cast<uint32_t>(raw[2]) << 10) |
                       (static_cast<uint32_t>(raw[3]) << 2) |
                       ((raw[6] >> 4) & 0x03);
    const uint32_t z = (static_cast<uint32_t>(raw[4]) << 10) |
                       (static_cast<uint32_t>(raw[5]) << 2) |
                       ((raw[6] >> 2) & 0x03);

    field->x_milligauss = (static_cast<float>(x) - RAW_CENTER) * MILLIGAUSS_PER_LSB;
    field->y_milligauss = (static_cast<float>(y) - RAW_CENTER) * MILLIGAUSS_PER_LSB;
    field->z_milligauss = (static_cast<float>(z) - RAW_CENTER) * MILLIGAUSS_PER_LSB;
    float heading = std::atan2(field->y_milligauss - y_offset_milligauss_,
                               field->x_milligauss - x_offset_milligauss_) *
                    180.0F / 3.14159265F;
    if (heading < 0.0F)
    {
        heading += 360.0F;
    }
    field->heading_degrees = heading;
    field->valid = true;
    return ESP_OK;
}

void MMC5983MA::setHardIronOffset(const float x_milligauss,
                                   const float y_milligauss)
{
    x_offset_milligauss_ = x_milligauss;
    y_offset_milligauss_ = y_milligauss;
}
