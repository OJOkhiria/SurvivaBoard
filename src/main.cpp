#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hal_i2c.hpp"
#include "hal_spi.hpp"
#include "hal_uart.hpp"

namespace
{
constexpr char TAG[] = "survivaboard";
}

extern "C" void app_main()
{
    ESP_ERROR_CHECK(HAL::I2C::init());
    ESP_ERROR_CHECK(HAL::SPI::init());
    ESP_ERROR_CHECK(HAL::UART::initGps());

    ESP_LOGI(TAG, "SurvivaBoard bus infrastructure initialized");

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}