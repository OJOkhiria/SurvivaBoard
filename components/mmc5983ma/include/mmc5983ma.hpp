#pragma once

#include <cstdint>

#include "esp_err.h"

struct MagneticField
{
    bool valid = false;
    float x_milligauss = 0.0F;
    float y_milligauss = 0.0F;
    float z_milligauss = 0.0F;
    float heading_degrees = 0.0F;
};

class MMC5983MA
{
public:
    static constexpr uint8_t ADDRESS = 0x30;
    static constexpr uint8_t PRODUCT_ID = 0x30;

    esp_err_t begin();
    bool isPresent() const;
    esp_err_t read(MagneticField* field);

    // Call this after a figure-eight / hard-iron calibration routine is added.
    // The defaults provide an uncalibrated but useful relative heading.
    void setHardIronOffset(float x_milligauss, float y_milligauss);

private:
    bool present_ = false;
    float x_offset_milligauss_ = 0.0F;
    float y_offset_milligauss_ = 0.0F;
};
