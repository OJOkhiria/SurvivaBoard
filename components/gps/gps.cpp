#include "gps.hpp"

#include "hal_uart.hpp"

esp_err_t GPS::begin()
{
    return HAL::UART::initGps();
}

int GPS::read(uint8_t* data, const size_t length, const uint32_t timeout_ms)
{
    return HAL::UART::readGps(data, length, timeout_ms);
}

int GPS::write(const uint8_t* data, const size_t length)
{
    return HAL::UART::writeGps(data, length);
}
