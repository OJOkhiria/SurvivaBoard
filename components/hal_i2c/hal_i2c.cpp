#include "hal_i2c.hpp"

#include "buses.hpp"
#include "config.hpp"
#include "pins.hpp"

namespace HAL::I2C
{
namespace
{
bool s_initialized = false;
}

esp_err_t init()
{
    if (s_initialized)
    {
        return ESP_OK;
    }

    const i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = Pins::I2C_SDA,
        .scl_io_num = Pins::I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master = {.clk_speed = Settings::I2C_SPEED},
        .clk_flags = 0,
    };
    esp_err_t result = i2c_param_config(Buses::I2C, &config);
    if (result == ESP_OK)
    {
        result = i2c_driver_install(Buses::I2C, config.mode, 0, 0, 0);
    }
    if (result == ESP_OK)
    {
        s_initialized = true;
    }
    return result;
}

esp_err_t readRegister(const uint8_t address, const uint8_t reg,
                       uint8_t* data, const size_t length, const int timeout_ms)
{
    return i2c_master_write_read_device(Buses::I2C, address, &reg, sizeof(reg),
                                        data, length, pdMS_TO_TICKS(timeout_ms));
}

esp_err_t writeRegister(const uint8_t address, const uint8_t reg,
                        const uint8_t* data, const size_t length,
                        const int timeout_ms)
{
    uint8_t buffer[1 + 32] = {reg};
    if (length > sizeof(buffer) - 1)
    {
        return ESP_ERR_INVALID_SIZE;
    }

    for (size_t i = 0; i < length; ++i)
    {
        buffer[i + 1] = data[i];
    }
    return i2c_master_write_to_device(Buses::I2C, address, buffer, length + 1,
                                      pdMS_TO_TICKS(timeout_ms));
}
}  // namespace HAL::I2C
