#include <cstdint>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
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
constexpr int64_t SENSOR_INTERVAL_US = 1'000'000;
constexpr int64_t DISPLAY_INTERVAL_US = 500'000;

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

    static Display display;
    static LoRa lora;
    static GPS gps;
    static BME688 environment;
    static MMC5983MA magnetometer;

    const esp_err_t display_result = display.begin();
    logInitialization("ILI9341 display", display_result);
    logInitialization("E22 LoRa interface", lora.begin());
    const esp_err_t gps_result = gps.begin();
    const esp_err_t environment_result = environment.begin();
    const esp_err_t magnetometer_result = magnetometer.begin();
    logInitialization("MAX-M10S UART", gps_result);
    logInitialization("BME688", environment_result);
    logInitialization("MMC5983MA", magnetometer_result);

    DisplayTelemetry telemetry = {};
    EnvironmentReading environment_reading = {};
    MagneticField magnetic_field = {};
    int64_t next_sensor_update = 0;
    int64_t next_display_update = 0;

    ESP_LOGI(TAG, "SurvivaBoard dashboard running");

    while (true)
    {
        if (gps_result == ESP_OK)
        {
            const esp_err_t result = gps.poll(0);
            if (result != ESP_OK)
            {
                ESP_LOGW(TAG, "GPS poll failed: %s", esp_err_to_name(result));
            }
        }

        const int64_t now = esp_timer_get_time();
        if (now >= next_sensor_update)
        {
            next_sensor_update = now + SENSOR_INTERVAL_US;
            if (environment_result == ESP_OK)
            {
                const esp_err_t result = environment.read(&environment_reading);
                if (result != ESP_OK)
                {
                    ESP_LOGW(TAG, "BME688 measurement failed: %s", esp_err_to_name(result));
                    environment_reading.valid = false;
                }
            }
            if (magnetometer_result == ESP_OK)
            {
                const esp_err_t result = magnetometer.read(&magnetic_field);
                if (result != ESP_OK)
                {
                    ESP_LOGW(TAG, "MMC5983MA measurement failed: %s", esp_err_to_name(result));
                    magnetic_field.valid = false;
                }
            }
        }

        if (display_result == ESP_OK && now >= next_display_update)
        {
            next_display_update = now + DISPLAY_INTERVAL_US;
            const GPSFix& fix = gps.fix();
            telemetry.gps_fix = fix.valid;
            telemetry.latitude = fix.latitude;
            telemetry.longitude = fix.longitude;
            telemetry.heading_valid = magnetic_field.valid;
            telemetry.heading_calibrated = false;
            telemetry.heading_degrees = magnetic_field.heading_degrees;
            telemetry.environment_valid = environment_reading.valid;
            telemetry.temperature_c = environment_reading.temperature_c;
            telemetry.humidity_percent = environment_reading.humidity_percent;
            telemetry.pressure_hpa = environment_reading.pressure_hpa;
            telemetry.gas_resistance_ohms = environment_reading.gas_resistance_ohms;
            telemetry.gas_valid = environment_reading.gas_valid;
            telemetry.heater_stable = environment_reading.heater_stable;
            telemetry.iaq_valid = environment_reading.iaq_valid;
            telemetry.iaq = environment_reading.iaq;

            const esp_err_t result = display.renderTelemetry(telemetry);
            if (result != ESP_OK)
            {
                ESP_LOGW(TAG, "Dashboard render failed: %s", esp_err_to_name(result));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(25));
    }
}
