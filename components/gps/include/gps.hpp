#pragma once

#include <cstddef>
#include <cstdint>

#include "esp_err.h"

struct GPSFix
{
    bool valid = false;
    double latitude = 0.0;
    double longitude = 0.0;
    float course_degrees = 0.0F;
    bool course_valid = false;
    uint8_t satellites = 0;
};

class GPS
{
public:
    esp_err_t begin();
    int read(uint8_t* data, size_t length, uint32_t timeout_ms = 100);
    int write(const uint8_t* data, size_t length);
    esp_err_t poll(uint32_t timeout_ms = 0);
    const GPSFix& fix() const;

private:
    void consume(char character);
    void parseSentence(char* sentence);
    static bool parseCoordinate(const char* value, const char* hemisphere,
                                double* decimal_degrees);

    GPSFix fix_ = {};
    char sentence_[128] = {};
    size_t sentence_length_ = 0;
};
