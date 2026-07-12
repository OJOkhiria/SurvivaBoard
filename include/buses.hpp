#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "driver/uart.h"

namespace Buses
{
    constexpr spi_host_device_t SPI = SPI2_HOST;

    constexpr i2c_port_t I2C = I2C_NUM_0;

    constexpr uart_port_t GPS = UART_NUM_1;
}
#pragma once
