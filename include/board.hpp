#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "driver/uart.h"

namespace Board
{
    constexpr char NAME[] = "SurvivaBoard";

    constexpr uint8_t VERSION_MAJOR = 1;
    constexpr uint8_t VERSION_MINOR = 0;

    constexpr float MAIN_SUPPLY = 3.3f;
}