#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "driver/uart.h"

namespace Settings
{
    constexpr uint32_t I2C_SPEED = 400000;

    constexpr uint32_t SPI_SPEED_DISPLAY = 40000000;

    constexpr uint32_t SPI_SPEED_LORA = 8000000;

    constexpr uint32_t GPS_BAUD = 9600;
}