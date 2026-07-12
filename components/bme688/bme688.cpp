#include "bme688.hpp"

#include <algorithm>
#include <cmath>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal_i2c.hpp"

namespace
{
constexpr uint8_t CHIP_ID_REGISTER = 0xD0;
constexpr uint8_t RESET_REGISTER = 0xE0;
constexpr uint8_t RESET_COMMAND = 0xB6;
constexpr uint8_t VARIANT_ID_REGISTER = 0xF0;
constexpr uint8_t CTRL_GAS_0_REGISTER = 0x70;
constexpr uint8_t CTRL_GAS_1_REGISTER = 0x71;
constexpr uint8_t CTRL_HUM_REGISTER = 0x72;
constexpr uint8_t CTRL_MEAS_REGISTER = 0x74;
constexpr uint8_t CONFIG_REGISTER = 0x75;
constexpr uint8_t RES_HEAT_0_REGISTER = 0x5A;
constexpr uint8_t GAS_WAIT_0_REGISTER = 0x64;
constexpr uint8_t MEASUREMENT_STATUS_REGISTER = 0x1D;
constexpr uint8_t FIELD_DATA_REGISTER = 0x1D;
constexpr uint8_t NEW_DATA_MASK = 0x80;
constexpr uint8_t GAS_VALID_MASK = 0x20;
constexpr uint8_t HEATER_STABLE_MASK = 0x10;
constexpr uint8_t VARIANT_GAS_HIGH = 0x01;

constexpr int32_t GAS_LOOKUP_1[16] = {
    2147483647, 2147483647, 2147483647, 2147483647,
    2147483647, 2126008810, 2147483647, 2130303777,
    2147483647, 2147483647, 2143188679, 2136746228,
    2147483647, 2126008810, 2147483647, 2147483647,
};
constexpr int32_t GAS_LOOKUP_2[16] = {
    409600000, 204800000, 102400000, 512000000,
    255744255, 127110228, 64000000, 32258064,
    16016016, 8000000, 4000000, 2000000,
    1000000, 500000, 250000, 125000,
};

uint8_t readByte(const uint8_t address, const uint8_t reg, esp_err_t* result)
{
    uint8_t value = 0;
    *result = HAL::I2C::readRegister(address, reg, &value, sizeof(value));
    return value;
}
}

esp_err_t BME688::begin()
{
    uint8_t chip_id = 0;
    esp_err_t result = HAL::I2C::readRegister(
        address_, CHIP_ID_REGISTER, &chip_id, sizeof(chip_id));
    if (result != ESP_OK || chip_id != CHIP_ID)
    {
        address_ = ADDRESS_HIGH;
        result = HAL::I2C::readRegister(address_, CHIP_ID_REGISTER, &chip_id,
                                        sizeof(chip_id));
    }
    present_ = result == ESP_OK && chip_id == CHIP_ID;
    if (!present_)
    {
        return ESP_ERR_NOT_FOUND;
    }

    result = HAL::I2C::writeRegister(address_, RESET_REGISTER, &RESET_COMMAND,
                                     sizeof(RESET_COMMAND));
    if (result != ESP_OK)
    {
        return result;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    result = HAL::I2C::readRegister(address_, VARIANT_ID_REGISTER, &variant_id_,
                                    sizeof(variant_id_));
    if (result != ESP_OK)
    {
        present_ = false;
        return result;
    }
    if ((result = readCalibration()) != ESP_OK)
    {
        present_ = false;
        return result;
    }
    return configure();
}

bool BME688::isPresent() const
{
    return present_;
}

esp_err_t BME688::read(EnvironmentReading* reading)
{
    if (reading == nullptr || !present_)
    {
        return ESP_ERR_INVALID_STATE;
    }

    // Forced mode starts one temperature, pressure, humidity, and gas cycle.
    const uint8_t forced_mode = 0x49;  // 2x T/P oversampling, forced mode.
    esp_err_t result = HAL::I2C::writeRegister(address_, CTRL_MEAS_REGISTER,
                                                &forced_mode, sizeof(forced_mode));
    if (result != ESP_OK)
    {
        return result;
    }

    uint8_t status = 0;
    for (int attempt = 0; attempt < 30; ++attempt)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
        result = HAL::I2C::readRegister(address_, MEASUREMENT_STATUS_REGISTER,
                                        &status, sizeof(status));
        if (result != ESP_OK)
        {
            return result;
        }
        if ((status & NEW_DATA_MASK) != 0)
        {
            break;
        }
    }
    if ((status & NEW_DATA_MASK) == 0)
    {
        return ESP_ERR_TIMEOUT;
    }

    uint8_t data[15] = {};
    result = HAL::I2C::readRegister(address_, FIELD_DATA_REGISTER, data,
                                    sizeof(data));
    if (result != ESP_OK)
    {
        return result;
    }
    const int32_t adc_pressure = (static_cast<int32_t>(data[2]) << 12) |
                                 (static_cast<int32_t>(data[3]) << 4) |
                                 (data[4] >> 4);
    const int32_t adc_temperature = (static_cast<int32_t>(data[5]) << 12) |
                                    (static_cast<int32_t>(data[6]) << 4) |
                                    (data[7] >> 4);
    const int32_t adc_humidity = (static_cast<int32_t>(data[8]) << 8) | data[9];

    const float temp_var1 = (static_cast<float>(adc_temperature) / 16384.0F -
                             static_cast<float>(calibration_.par_t1) / 1024.0F) *
                            static_cast<float>(calibration_.par_t2);
    const float temp_var2 = ((static_cast<float>(adc_temperature) / 131072.0F -
                              static_cast<float>(calibration_.par_t1) / 8192.0F) *
                             (static_cast<float>(adc_temperature) / 131072.0F -
                              static_cast<float>(calibration_.par_t1) / 8192.0F)) *
                            (static_cast<float>(calibration_.par_t3) * 16.0F);
    const float t_fine = temp_var1 + temp_var2;
    reading->temperature_c = t_fine / 5120.0F;

    float pressure_var1 = t_fine / 2.0F - 64000.0F;
    float pressure_var2 = pressure_var1 * pressure_var1 *
                          static_cast<float>(calibration_.par_p6) / 131072.0F;
    pressure_var2 += pressure_var1 * static_cast<float>(calibration_.par_p5) * 2.0F;
    pressure_var2 = pressure_var2 / 4.0F + static_cast<float>(calibration_.par_p4) * 65536.0F;
    pressure_var1 = (static_cast<float>(calibration_.par_p3) * pressure_var1 * pressure_var1 /
                     16384.0F + static_cast<float>(calibration_.par_p2) * pressure_var1) /
                    524288.0F;
    pressure_var1 = (1.0F + pressure_var1 / 32768.0F) *
                    static_cast<float>(calibration_.par_p1);
    if (std::fabs(pressure_var1) < 0.001F)
    {
        return ESP_ERR_INVALID_RESPONSE;
    }
    float pressure = 1048576.0F - static_cast<float>(adc_pressure);
    pressure = (pressure - pressure_var2 / 4096.0F) * 6250.0F / pressure_var1;
    pressure_var1 = static_cast<float>(calibration_.par_p9) * pressure * pressure /
                    2147483648.0F;
    pressure_var2 = pressure * static_cast<float>(calibration_.par_p8) / 32768.0F;
    const float pressure_var3 = pressure * pressure * pressure *
                                static_cast<float>(calibration_.par_p10) /
                                131072000.0F;
    pressure += (pressure_var1 + pressure_var2 + pressure_var3 +
                 static_cast<float>(calibration_.par_p7) * 128.0F) /
                16.0F;
    reading->pressure_hpa = pressure / 100.0F;

    const float humidity_var1 = static_cast<float>(adc_humidity) -
                                (static_cast<float>(calibration_.par_h1) * 16.0F +
                                 (static_cast<float>(calibration_.par_h3) / 2.0F) *
                                     reading->temperature_c);
    const float humidity_var2 = humidity_var1 *
        (static_cast<float>(calibration_.par_h2) / 262144.0F) *
        (1.0F + (static_cast<float>(calibration_.par_h4) / 16384.0F) *
                    reading->temperature_c +
         (static_cast<float>(calibration_.par_h5) / 1048576.0F) *
                    reading->temperature_c * reading->temperature_c);
    const float humidity_var3 = static_cast<float>(calibration_.par_h6) / 16384.0F +
                                static_cast<float>(calibration_.par_h7) /
                                    2097152.0F * reading->temperature_c;
    reading->humidity_percent = std::clamp(humidity_var2 * (1.0F -
        humidity_var3 * humidity_var2), 0.0F, 100.0F);

    const uint16_t adc_gas = (static_cast<uint16_t>(data[13]) << 2) |
                             (data[14] >> 6);
    const uint8_t gas_range = data[14] & 0x0F;
    reading->gas_valid = (data[14] & GAS_VALID_MASK) != 0;
    reading->heater_stable = (data[14] & HEATER_STABLE_MASK) != 0;
    if (reading->gas_valid && gas_range < 16)
    {
        if (variant_id_ == VARIANT_GAS_HIGH)
        {
            const int32_t gas_var1 = 262144 >> gas_range;
            const int32_t gas_var2 = 4096 +
                (static_cast<int32_t>(adc_gas) - 512) * 3;
            reading->gas_resistance_ohms = gas_var2 == 0 ? 0.0F :
                1000000.0F * static_cast<float>(gas_var1) /
                static_cast<float>(gas_var2);
        }
        else
        {
            const int64_t gas_var1 = (1340 + 5 * calibration_.range_sw_err) *
                                     static_cast<int64_t>(GAS_LOOKUP_1[gas_range]) / 65536;
            const int64_t gas_var2 = static_cast<int64_t>(adc_gas) * 32768 - 16777216 + gas_var1;
            const int64_t gas_var3 = static_cast<int64_t>(GAS_LOOKUP_2[gas_range]) * gas_var1 / 512;
            reading->gas_resistance_ohms = gas_var2 == 0 ? 0.0F :
                static_cast<float>(gas_var3) / static_cast<float>(gas_var2);
        }
    }
    else
    {
        reading->gas_resistance_ohms = 0.0F;
    }
    reading->iaq_valid = false;
    reading->iaq = 0.0F;
    reading->valid = true;
    return ESP_OK;
}

esp_err_t BME688::readCalibration()
{
    esp_err_t result = ESP_OK;
    auto read16 = [this, &result](const uint8_t reg) {
        uint8_t bytes[2] = {};
        result = HAL::I2C::readRegister(address_, reg, bytes, sizeof(bytes));
        return static_cast<uint16_t>(bytes[0]) |
               (static_cast<uint16_t>(bytes[1]) << 8);
    };
    auto read8 = [this, &result](const uint8_t reg) {
        return readByte(address_, reg, &result);
    };

    calibration_.par_t1 = read16(0xE9); if (result != ESP_OK) return result;
    calibration_.par_t2 = static_cast<int16_t>(read16(0x8A)); if (result != ESP_OK) return result;
    calibration_.par_t3 = static_cast<int8_t>(read8(0x8C)); if (result != ESP_OK) return result;
    calibration_.par_p1 = read16(0x8E); if (result != ESP_OK) return result;
    calibration_.par_p2 = static_cast<int16_t>(read16(0x90)); if (result != ESP_OK) return result;
    calibration_.par_p3 = static_cast<int8_t>(read8(0x92)); if (result != ESP_OK) return result;
    calibration_.par_p4 = static_cast<int16_t>(read16(0x94)); if (result != ESP_OK) return result;
    calibration_.par_p5 = static_cast<int16_t>(read16(0x96)); if (result != ESP_OK) return result;
    calibration_.par_p6 = static_cast<int8_t>(read8(0x99)); if (result != ESP_OK) return result;
    calibration_.par_p7 = static_cast<int8_t>(read8(0x98)); if (result != ESP_OK) return result;
    calibration_.par_p8 = static_cast<int16_t>(read16(0x9C)); if (result != ESP_OK) return result;
    calibration_.par_p9 = static_cast<int16_t>(read16(0x9E)); if (result != ESP_OK) return result;
    calibration_.par_p10 = read8(0xA0); if (result != ESP_OK) return result;

    const uint8_t h2_lsb = read8(0xE2); if (result != ESP_OK) return result;
    calibration_.par_h1 = static_cast<uint16_t>(read8(0xE3) << 4) | (h2_lsb & 0x0F);
    if (result != ESP_OK) return result;
    calibration_.par_h2 = static_cast<uint16_t>(read8(0xE1) << 4) | (h2_lsb >> 4);
    if (result != ESP_OK) return result;
    calibration_.par_h3 = static_cast<int8_t>(read8(0xE4)); if (result != ESP_OK) return result;
    calibration_.par_h4 = static_cast<int8_t>(read8(0xE5)); if (result != ESP_OK) return result;
    calibration_.par_h5 = static_cast<int8_t>(read8(0xE6)); if (result != ESP_OK) return result;
    calibration_.par_h6 = read8(0xE7); if (result != ESP_OK) return result;
    calibration_.par_h7 = static_cast<int8_t>(read8(0xE8)); if (result != ESP_OK) return result;
    calibration_.par_gh1 = static_cast<int8_t>(read8(0xED)); if (result != ESP_OK) return result;
    calibration_.par_gh2 = static_cast<int16_t>(read16(0xEB)); if (result != ESP_OK) return result;
    calibration_.par_gh3 = static_cast<int8_t>(read8(0xEE)); if (result != ESP_OK) return result;

    const uint8_t heat_range = read8(0x02); if (result != ESP_OK) return result;
    calibration_.res_heat_range = (heat_range & 0x30) >> 4;
    calibration_.res_heat_val = static_cast<int8_t>(read8(0x00)); if (result != ESP_OK) return result;
    calibration_.range_sw_err = static_cast<int8_t>(read8(0x04));
    calibration_.range_sw_err = static_cast<int8_t>((calibration_.range_sw_err & 0xF0) >> 4);
    return result;
}

esp_err_t BME688::configure()
{
    // 2x humidity oversampling, IIR filter 3, gas conversion enabled.
    const uint8_t humidity = 0x02;
    const uint8_t filter = 0x0C;
    const uint8_t heater_enabled = 0x08;
    const uint8_t gas_enabled = variant_id_ == VARIANT_GAS_HIGH ? 0x20 : 0x10;
    const uint8_t heater = calculateHeaterResistance(320, 25);
    const uint8_t gas_wait = 0xA5;  // ~150 ms, encoded per the BME68x data sheet.
    esp_err_t result = HAL::I2C::writeRegister(address_, CTRL_HUM_REGISTER,
                                                &humidity, sizeof(humidity));
    if (result == ESP_OK) result = HAL::I2C::writeRegister(address_, CONFIG_REGISTER,
                                                            &filter, sizeof(filter));
    if (result == ESP_OK) result = HAL::I2C::writeRegister(address_, CTRL_GAS_0_REGISTER,
                                                            &heater_enabled, sizeof(heater_enabled));
    if (result == ESP_OK) result = HAL::I2C::writeRegister(address_, CTRL_GAS_1_REGISTER,
                                                            &gas_enabled, sizeof(gas_enabled));
    if (result == ESP_OK) result = HAL::I2C::writeRegister(address_, RES_HEAT_0_REGISTER,
                                                            &heater, sizeof(heater));
    if (result == ESP_OK) result = HAL::I2C::writeRegister(address_, GAS_WAIT_0_REGISTER,
                                                            &gas_wait, sizeof(gas_wait));
    return result;
}

uint8_t BME688::calculateHeaterResistance(const uint16_t target_temperature_c,
                                          const int16_t ambient_temperature_c) const
{
    const int32_t var1 = (static_cast<int32_t>(ambient_temperature_c) *
                          calibration_.par_gh3 / 1000) * 256;
    const int32_t var2 = (static_cast<int32_t>(calibration_.par_gh1) + 784) *
                         (((static_cast<int32_t>(calibration_.par_gh2) + 154009) *
                           target_temperature_c * 5 / 100) + 3276800) / 10;
    const int32_t var3 = var1 + var2 / 2;
    const int32_t var4 = var3 / (static_cast<int32_t>(calibration_.res_heat_range) + 4);
    const int32_t var5 = 131 * static_cast<int32_t>(calibration_.res_heat_val) + 65536;
    const int32_t resistance = (((var4 / var5) - 250) * 34 + 50) / 100;
    return static_cast<uint8_t>(std::clamp<int32_t>(resistance, 0, 255));
}
