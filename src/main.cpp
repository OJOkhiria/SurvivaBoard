#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bme688.hpp"
#include "display.hpp"
#include "gps.hpp"
#include "hal_i2c.hpp"
#include "hal_spi.hpp"
#include "hal_uart.hpp"
#include "lora.hpp"
#include "mmc5983ma.hpp"

namespace
{
constexpr char TAG[] = "survivaboard";
void logInitialization(const char* name, const esp_err_t result)
{
    if (result == ESP_OK)
    {
        ESP_LOGI(TAG, "%s ready", name);
        return;
    }

    ESP_LOGW(TAG, "%s unavailable: %s", name, esp_err_to_name(result));
}
}

extern "C" void app_main()
{
    ESP_ERROR_CHECK(HAL::I2C::init());
    ESP_ERROR_CHECK(HAL::SPI::init());

    Display display;
    LoRa lora;
    GPS gps;
    BME688 environment;
    MMC5983MA magnetometer;

    logInitialization("ILI9341 SPI interface", display.begin());
    logInitialization("E22 LoRa interface", lora.begin());
    logInitialization("MAX-M10S UART", gps.begin());
    logInitialization("BME688", environment.begin());
    logInitialization("MMC5983MA", magnetometer.begin());

    ESP_LOGI(TAG, "SurvivaBoard bring-up complete");

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));    }

    }