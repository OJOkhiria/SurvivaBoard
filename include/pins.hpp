#pragma once

#include "driver/gpio.h"
namespace Pins
{
    //==========================
    // USB
    //==========================

    constexpr gpio_num_t USB_D_MINUS = GPIO_NUM_19;
    constexpr gpio_num_t USB_D_PLUS  = GPIO_NUM_20;

    //==========================
    // SPI Bus
    //==========================

    constexpr gpio_num_t SPI_SCK  = GPIO_NUM_4;
    constexpr gpio_num_t SPI_MOSI = GPIO_NUM_5;
    constexpr gpio_num_t SPI_MISO = GPIO_NUM_6;

    //==========================
    // LoRa (E22-900M22S)
    //==========================

    constexpr gpio_num_t LORA_CS    = GPIO_NUM_7;
    constexpr gpio_num_t LORA_RESET = GPIO_NUM_3;
    constexpr gpio_num_t LORA_TXEN  = GPIO_NUM_9;
    constexpr gpio_num_t LORA_RXEN  = GPIO_NUM_10;
    constexpr gpio_num_t LORA_BUSY  = GPIO_NUM_15;
    constexpr gpio_num_t LORA_DIO1  = GPIO_NUM_16;

    //==========================
    // GPS
    //==========================

    constexpr gpio_num_t GPS_TX  = GPIO_NUM_43;
    constexpr gpio_num_t GPS_RX  = GPIO_NUM_44;

    constexpr gpio_num_t GPS_TIMEPULSE = GPIO_NUM_47;
    constexpr gpio_num_t GPS_EXTINT    = GPIO_NUM_8;

    //==========================
    // I2C
    //==========================

    constexpr gpio_num_t I2C_SDA = GPIO_NUM_11;
    constexpr gpio_num_t I2C_SCL = GPIO_NUM_12;

    //==========================
    // Display (ILI9341)
    //==========================

    constexpr gpio_num_t TFT_CS   = GPIO_NUM_17;
    constexpr gpio_num_t TFT_DC   = GPIO_NUM_18;
    constexpr gpio_num_t TFT_RST  = GPIO_NUM_21;
    constexpr gpio_num_t TFT_PWM  = GPIO_NUM_48;

    //==========================
    // SD card (on the TFT module)
    //==========================

    constexpr gpio_num_t SD_CS = GPIO_NUM_13;

    //==========================
    // Magnetometer
    //==========================

    constexpr gpio_num_t MAG_INT = GPIO_NUM_38;
}
