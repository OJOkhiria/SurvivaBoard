#pragma once

#include <cstddef>
#include <cstdint>

#include "driver/spi_master.h"
#include "esp_err.h"

// Values consumed by the first on-device status screen.  The display component
// deliberately owns no sensor drivers; main.cpp assembles this snapshot from
// the GPS, magnetometer, and environmental sensor components.
struct DisplayTelemetry
{
    bool gps_fix = false;
    double latitude = 0.0;
    double longitude = 0.0;

    bool heading_valid = false;
    bool heading_calibrated = false;
    float heading_degrees = 0.0F;

    bool environment_valid = false;
    float temperature_c = 0.0F;
    float humidity_percent = 0.0F;
    float pressure_hpa = 0.0F;
    float gas_resistance_ohms = 0.0F;
    bool gas_valid = false;
    bool heater_stable = false;

    // A BME688 alone does not produce a Bosch IAQ index.  This stays false
    // until a BSEC/BSEC2 integration supplies a calibrated IAQ value.
    bool iaq_valid = false;
    float iaq = 0.0F;
};

class Display
{
public:
    esp_err_t begin();
    esp_err_t renderTelemetry(const DisplayTelemetry& telemetry);

private:
    static constexpr uint16_t WIDTH = 320;
    static constexpr uint16_t HEIGHT = 240;

    esp_err_t writeCommand(uint8_t command);
    esp_err_t writeData(const uint8_t* data, size_t length);
    esp_err_t setAddressWindow(uint16_t x, uint16_t y, uint16_t width,
                               uint16_t height);
    esp_err_t fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                       uint16_t color);
    esp_err_t drawLine(int x0, int y0, int x1, int y1, uint16_t color);
    esp_err_t drawCircle(int center_x, int center_y, int radius,
                         uint16_t color);
    esp_err_t drawText(uint16_t x, uint16_t y, const char* text, uint8_t scale,
                       uint16_t foreground, uint16_t background);

    spi_device_handle_t device_ = nullptr;
    bool initialized_ = false;
};
