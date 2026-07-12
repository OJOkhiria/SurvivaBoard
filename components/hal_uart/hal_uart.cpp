#include "hal_uart.hpp"

#include "buses.hpp"
#include "config.hpp"
#include "driver/uart.h"
#include "pins.hpp"

namespace HAL::UART
{
esp_err_t initGps()
{
    const uart_config_t config = {
        .baud_rate = static_cast<int>(Settings::GPS_BAUD),
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
        .flags = {},
    };

    esp_err_t result = uart_param_config(Buses::GPS, &config);
    if (result != ESP_OK)
    {
        return result;
    }

    result = uart_set_pin(Buses::GPS, Pins::GPS_TX, Pins::GPS_RX,
                          UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (result != ESP_OK)
    {
        return result;
    }

    return uart_driver_install(Buses::GPS, Settings::GPS_UART_RX_BUFFER_SIZE,
                               Settings::GPS_UART_TX_BUFFER_SIZE, 0, nullptr,
                               0);
}

int readGps(uint8_t* data, const size_t length, const uint32_t timeout_ms)
{
    return uart_read_bytes(Buses::GPS, data, length,
                           pdMS_TO_TICKS(timeout_ms));
}

int writeGps(const uint8_t* data, const size_t length)
{
    return uart_write_bytes(Buses::GPS,
                            reinterpret_cast<const char*>(data), length);
}
}  // namespace HAL::UART
