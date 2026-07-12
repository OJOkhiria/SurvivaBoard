#pragma once

#include "driver/gpio.h"

namespace board::pins {

inline constexpr gpio_num_t spi_sck  = GPIO_NUM_4;
inline constexpr gpio_num_t spi_mosi = GPIO_NUM_5;
inline constexpr gpio_num_t spi_miso = GPIO_NUM_6;

inline constexpr gpio_num_t lora_reset = GPIO_NUM_3;
inline constexpr gpio_num_t lora_cs    = GPIO_NUM_7;
inline constexpr gpio_num_t gps_extint = GPIO_NUM_8;
inline constexpr gpio_num_t lora_txen  = GPIO_NUM_9;
inline constexpr gpio_num_t lora_rxen  = GPIO_NUM_10;

inline constexpr gpio_num_t i2c_sda = GPIO_NUM_11;
inline constexpr gpio_num_t i2c_scl = GPIO_NUM_12;
inline constexpr gpio_num_t sd_cs = GPIO_NUM_13;

inline constexpr gpio_num_t lora_busy = GPIO_NUM_15;
inline constexpr gpio_num_t lora_dio1 = GPIO_NUM_16;

inline constexpr gpio_num_t tft_cs  = GPIO_NUM_17;
inline constexpr gpio_num_t tft_dc  = GPIO_NUM_18;
inline constexpr gpio_num_t tft_rst = GPIO_NUM_21;
inline constexpr gpio_num_t mag_int = GPIO_NUM_38;

inline constexpr gpio_num_t gps_timepulse = GPIO_NUM_47;
inline constexpr gpio_num_t tft_pwm       = GPIO_NUM_48;

inline constexpr gpio_num_t gps_rx = GPIO_NUM_44; // ESP32 RX <- GPS TX
inline constexpr gpio_num_t gps_tx = GPIO_NUM_43; // ESP32 TX -> GPS RX

} // namespace board::pins
