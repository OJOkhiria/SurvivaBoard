#pragma once

#include <cstdint>

#include "esp_err.h"

struct EnvironmentReading
{
    bool valid = false;
    float temperature_c = 0.0F;
    float humidity_percent = 0.0F;
    float pressure_hpa = 0.0F;
    float gas_resistance_ohms = 0.0F;
    bool gas_valid = false;
    bool heater_stable = false;

    // Bosch's IAQ is a BSEC/BSEC2 output, not a direct BME688 measurement.
    // Keep it unavailable until BSEC is added to the project.
    bool iaq_valid = false;
    float iaq = 0.0F;
};

class BME688
{
public:
    static constexpr uint8_t ADDRESS = 0x76;
    static constexpr uint8_t ADDRESS_HIGH = 0x77;
    static constexpr uint8_t CHIP_ID = 0x61;

    esp_err_t begin();
    bool isPresent() const;
    esp_err_t read(EnvironmentReading* reading);

private:
    struct Calibration
    {
        uint16_t par_t1 = 0;
        int16_t par_t2 = 0;
        int8_t par_t3 = 0;
        uint16_t par_p1 = 0;
        int16_t par_p2 = 0;
        int8_t par_p3 = 0;
        int16_t par_p4 = 0;
        int16_t par_p5 = 0;
        int8_t par_p6 = 0;
        int8_t par_p7 = 0;
        int16_t par_p8 = 0;
        int16_t par_p9 = 0;
        uint8_t par_p10 = 0;
        uint16_t par_h1 = 0;
        uint16_t par_h2 = 0;
        int8_t par_h3 = 0;
        int8_t par_h4 = 0;
        int8_t par_h5 = 0;
        uint8_t par_h6 = 0;
        int8_t par_h7 = 0;
        int8_t par_gh1 = 0;
        int16_t par_gh2 = 0;
        int8_t par_gh3 = 0;
        uint8_t res_heat_range = 0;
        int8_t res_heat_val = 0;
        int8_t range_sw_err = 0;
    };

    esp_err_t readCalibration();
    esp_err_t configure();
    uint8_t calculateHeaterResistance(uint16_t target_temperature_c,
                                      int16_t ambient_temperature_c) const;
    bool present_ = false;
    uint8_t address_ = ADDRESS;
    uint8_t variant_id_ = 0;
    Calibration calibration_ = {};
};
