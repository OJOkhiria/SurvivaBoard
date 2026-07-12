#include "gps.hpp"

#include <cstdlib>
#include <cstring>

#include "driver/gpio.h"
#include "hal_uart.hpp"
#include "pins.hpp"

esp_err_t GPS::begin()
{
    const gpio_config_t timepulse_config = {
        .pin_bit_mask = 1ULL << board::pins::gps_timepulse,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t result = gpio_config(&timepulse_config);
    if (result != ESP_OK)
    {
        return result;
    }

    const gpio_config_t extint_config = {
        .pin_bit_mask = 1ULL << board::pins::gps_extint,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    result = gpio_config(&extint_config);
    if (result != ESP_OK)
    {
        return result;
    }
    gpio_set_level(board::pins::gps_extint, 0);

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

esp_err_t GPS::poll(const uint32_t timeout_ms)
{
    uint8_t buffer[128] = {};
    const int received = read(buffer, sizeof(buffer), timeout_ms);
    if (received < 0)
    {
        return ESP_FAIL;
    }
    for (int index = 0; index < received; ++index)
    {
        consume(static_cast<char>(buffer[index]));
    }
    return ESP_OK;
}

const GPSFix& GPS::fix() const
{
    return fix_;
}

void GPS::consume(const char character)
{
    if (character == '\r')
    {
        return;
    }
    if (character == '\n')
    {
        if (sentence_length_ > 0)
        {
            sentence_[sentence_length_] = '\0';
            parseSentence(sentence_);
        }
        sentence_length_ = 0;
        return;
    }
    if (character == '$')
    {
        sentence_length_ = 0;
    }
    if (sentence_length_ + 1 < sizeof(sentence_))
    {
        sentence_[sentence_length_++] = character;
    }
    else
    {
        sentence_length_ = 0;
    }
}

void GPS::parseSentence(char* sentence)
{
    if (sentence[0] != '$')
    {
        return;
    }

    char* checksum = std::strchr(sentence, '*');
    if (checksum != nullptr)
    {
        *checksum = '\0';
    }

    char* fields[16] = {};
    size_t field_count = 0;
    char* save = nullptr;
    for (char* token = strtok_r(sentence, ",", &save);
         token != nullptr && field_count < 16;
         token = strtok_r(nullptr, ",", &save))
    {
        fields[field_count++] = token;
    }
    if (field_count < 2 || std::strlen(fields[0]) < 6)
    {
        return;
    }

    const char* type = fields[0] + std::strlen(fields[0]) - 3;
    if (std::strcmp(type, "GGA") == 0 && field_count >= 8)
    {
        const int quality = std::atoi(fields[6]);
        fix_.satellites = static_cast<uint8_t>(std::atoi(fields[7]));
        if (quality > 0 && parseCoordinate(fields[2], fields[3], &fix_.latitude) &&
            parseCoordinate(fields[4], fields[5], &fix_.longitude))
        {
            fix_.valid = true;
        }
        else if (quality == 0)
        {
            fix_.valid = false;
        }
    }
    else if (std::strcmp(type, "RMC") == 0 && field_count >= 9)
    {
        if (fields[2][0] != 'A')
        {
            fix_.valid = false;
            return;
        }
        if (parseCoordinate(fields[3], fields[4], &fix_.latitude) &&
            parseCoordinate(fields[5], fields[6], &fix_.longitude))
        {
            fix_.valid = true;
        }
        if (fields[8][0] != '\0')
        {
            fix_.course_degrees = std::strtof(fields[8], nullptr);
            fix_.course_valid = fix_.course_degrees >= 0.0F && fix_.course_degrees < 360.0F;
        }
    }
}

bool GPS::parseCoordinate(const char* value, const char* hemisphere,
                          double* decimal_degrees)
{
    if (value == nullptr || hemisphere == nullptr || value[0] == '\0' ||
        hemisphere[0] == '\0')
    {
        return false;
    }
    const double raw = std::strtod(value, nullptr);
    const int degrees = static_cast<int>(raw / 100.0);
    double result = static_cast<double>(degrees) + (raw - degrees * 100.0) / 60.0;
    if (hemisphere[0] == 'S' || hemisphere[0] == 'W')
    {
        result = -result;
    }
    if (hemisphere[0] != 'N' && hemisphere[0] != 'S' &&
        hemisphere[0] != 'E' && hemisphere[0] != 'W')
    {
        return false;
    }
    *decimal_degrees = result;
    return true;
}
