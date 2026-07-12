#include "display.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

#include "config.hpp"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal_spi.hpp"
#include "pins.hpp"

namespace
{
constexpr uint16_t BLACK = 0x0000;
constexpr uint16_t NAVY = 0x000F;
constexpr uint16_t BLUE = 0x001F;
constexpr uint16_t CYAN = 0x07FF;
constexpr uint16_t GREEN = 0x07E0;
constexpr uint16_t YELLOW = 0xFFE0;
constexpr uint16_t ORANGE = 0xFD20;
constexpr uint16_t WHITE = 0xFFFF;
constexpr uint16_t GRAY = 0x8410;
constexpr uint16_t RED = 0xF800;

constexpr uint8_t ILI9341_SWRESET = 0x01;
constexpr uint8_t ILI9341_SLPOUT = 0x11;
constexpr uint8_t ILI9341_DISPOFF = 0x28;
constexpr uint8_t ILI9341_DISPON = 0x29;
constexpr uint8_t ILI9341_CASET = 0x2A;
constexpr uint8_t ILI9341_PASET = 0x2B;
constexpr uint8_t ILI9341_RAMWR = 0x2C;
constexpr uint8_t ILI9341_MADCTL = 0x36;
constexpr uint8_t ILI9341_PIXFMT = 0x3A;

// Compact 3x5 font. Keeping the bring-up UI self-contained avoids an SD-card
// or external font dependency while still making all telemetry readable.
// Each entry uses the low three bits for a row, top to bottom.
void glyphRows(const char character, uint8_t rows[5])
{
    std::memset(rows, 0, 5);
    const char c = character >= 'a' && character <= 'z'
                       ? static_cast<char>(character - 'a' + 'A')
                       : character;
    switch (c)
    {
    case '0': { const uint8_t g[] = {7, 5, 5, 5, 7}; std::memcpy(rows, g, 5); break; }
    case '1': { const uint8_t g[] = {2, 6, 2, 2, 7}; std::memcpy(rows, g, 5); break; }
    case '2': { const uint8_t g[] = {6, 1, 7, 4, 7}; std::memcpy(rows, g, 5); break; }
    case '3': { const uint8_t g[] = {6, 1, 3, 1, 6}; std::memcpy(rows, g, 5); break; }
    case '4': { const uint8_t g[] = {5, 5, 7, 1, 1}; std::memcpy(rows, g, 5); break; }
    case '5': { const uint8_t g[] = {7, 4, 6, 1, 6}; std::memcpy(rows, g, 5); break; }
    case '6': { const uint8_t g[] = {3, 4, 7, 5, 7}; std::memcpy(rows, g, 5); break; }
    case '7': { const uint8_t g[] = {7, 1, 2, 2, 2}; std::memcpy(rows, g, 5); break; }
    case '8': { const uint8_t g[] = {7, 5, 7, 5, 7}; std::memcpy(rows, g, 5); break; }
    case '9': { const uint8_t g[] = {7, 5, 7, 1, 6}; std::memcpy(rows, g, 5); break; }
    case 'A': { const uint8_t g[] = {2, 5, 7, 5, 5}; std::memcpy(rows, g, 5); break; }
    case 'B': { const uint8_t g[] = {6, 5, 6, 5, 6}; std::memcpy(rows, g, 5); break; }
    case 'C': { const uint8_t g[] = {3, 4, 4, 4, 3}; std::memcpy(rows, g, 5); break; }
    case 'D': { const uint8_t g[] = {6, 5, 5, 5, 6}; std::memcpy(rows, g, 5); break; }
    case 'E': { const uint8_t g[] = {7, 4, 6, 4, 7}; std::memcpy(rows, g, 5); break; }
    case 'F': { const uint8_t g[] = {7, 4, 6, 4, 4}; std::memcpy(rows, g, 5); break; }
    case 'G': { const uint8_t g[] = {3, 4, 5, 5, 3}; std::memcpy(rows, g, 5); break; }
    case 'H': { const uint8_t g[] = {5, 5, 7, 5, 5}; std::memcpy(rows, g, 5); break; }
    case 'I': { const uint8_t g[] = {7, 2, 2, 2, 7}; std::memcpy(rows, g, 5); break; }
    case 'J': { const uint8_t g[] = {1, 1, 1, 5, 2}; std::memcpy(rows, g, 5); break; }
    case 'K': { const uint8_t g[] = {5, 5, 6, 5, 5}; std::memcpy(rows, g, 5); break; }
    case 'L': { const uint8_t g[] = {4, 4, 4, 4, 7}; std::memcpy(rows, g, 5); break; }
    case 'M': { const uint8_t g[] = {5, 7, 7, 5, 5}; std::memcpy(rows, g, 5); break; }
    case 'N': { const uint8_t g[] = {5, 7, 7, 7, 5}; std::memcpy(rows, g, 5); break; }
    case 'O': { const uint8_t g[] = {2, 5, 5, 5, 2}; std::memcpy(rows, g, 5); break; }
    case 'P': { const uint8_t g[] = {6, 5, 6, 4, 4}; std::memcpy(rows, g, 5); break; }
    case 'Q': { const uint8_t g[] = {2, 5, 5, 7, 3}; std::memcpy(rows, g, 5); break; }
    case 'R': { const uint8_t g[] = {6, 5, 6, 5, 5}; std::memcpy(rows, g, 5); break; }
    case 'S': { const uint8_t g[] = {3, 4, 2, 1, 6}; std::memcpy(rows, g, 5); break; }
    case 'T': { const uint8_t g[] = {7, 2, 2, 2, 2}; std::memcpy(rows, g, 5); break; }
    case 'U': { const uint8_t g[] = {5, 5, 5, 5, 7}; std::memcpy(rows, g, 5); break; }
    case 'V': { const uint8_t g[] = {5, 5, 5, 5, 2}; std::memcpy(rows, g, 5); break; }
    case 'W': { const uint8_t g[] = {5, 5, 7, 7, 5}; std::memcpy(rows, g, 5); break; }
    case 'X': { const uint8_t g[] = {5, 5, 2, 5, 5}; std::memcpy(rows, g, 5); break; }
    case 'Y': { const uint8_t g[] = {5, 5, 2, 2, 2}; std::memcpy(rows, g, 5); break; }
    case 'Z': { const uint8_t g[] = {7, 1, 2, 4, 7}; std::memcpy(rows, g, 5); break; }
    case '.': { rows[4] = 2; break; }
    case ':': { rows[1] = 2; rows[3] = 2; break; }
    case '-': { rows[2] = 7; break; }
    case '/': { const uint8_t g[] = {1, 1, 2, 4, 4}; std::memcpy(rows, g, 5); break; }
    case '+': { const uint8_t g[] = {0, 2, 7, 2, 0}; std::memcpy(rows, g, 5); break; }
    default: break;
    }
}

const char* cardinalDirection(float degrees)
{
    static constexpr const char* DIRECTIONS[] = {
        "N", "NE", "E", "SE", "S", "SW", "W", "NW"
    };
    const int index = static_cast<int>(std::floor((degrees + 22.5F) / 45.0F)) % 8;
    return DIRECTIONS[index < 0 ? index + 8 : index];
}
}  // namespace

esp_err_t Display::begin()
{
    const gpio_config_t control_pins = {
        .pin_bit_mask = (1ULL << board::pins::tft_dc) |
                        (1ULL << board::pins::tft_rst) |
                        (1ULL << board::pins::tft_pwm),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t result = gpio_config(&control_pins);
    if (result != ESP_OK)
    {
        return result;
    }

    gpio_set_level(board::pins::tft_dc, 1);
    gpio_set_level(board::pins::tft_rst, 0);
    // The CR2013-MI2120 module accepts a logic-high backlight-enable signal.
    gpio_set_level(board::pins::tft_pwm, 1);

    result = HAL::SPI::addDevice(Settings::SPI_SPEED_DISPLAY, 7,
                                 board::pins::tft_cs, &device_);
    if (result != ESP_OK)
    {
        return result;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(board::pins::tft_rst, 1);
    vTaskDelay(pdMS_TO_TICKS(120));

    if ((result = writeCommand(ILI9341_SWRESET)) != ESP_OK)
    {
        return result;
    }
    vTaskDelay(pdMS_TO_TICKS(150));
    if ((result = writeCommand(ILI9341_DISPOFF)) != ESP_OK ||
        (result = writeCommand(ILI9341_PIXFMT)) != ESP_OK)
    {
        return result;
    }
    const uint8_t pixel_format = 0x55;  // RGB565
    if ((result = writeData(&pixel_format, sizeof(pixel_format))) != ESP_OK ||
        (result = writeCommand(ILI9341_MADCTL)) != ESP_OK)
    {
        return result;
    }
    // Landscape orientation with the controller's BGR color order.
    const uint8_t orientation = 0x28;
    if ((result = writeData(&orientation, sizeof(orientation))) != ESP_OK ||
        (result = writeCommand(ILI9341_SLPOUT)) != ESP_OK)
    {
        return result;
    }
    vTaskDelay(pdMS_TO_TICKS(120));
    if ((result = writeCommand(ILI9341_DISPON)) != ESP_OK)
    {
        return result;
    }
    vTaskDelay(pdMS_TO_TICKS(20));

    initialized_ = true;
    return fillRect(0, 0, WIDTH, HEIGHT, BLACK);
}

esp_err_t Display::writeCommand(const uint8_t command)
{
    gpio_set_level(board::pins::tft_dc, 0);
    return HAL::SPI::transmit(device_, &command, sizeof(command));
}

esp_err_t Display::writeData(const uint8_t* data, const size_t length)
{
    gpio_set_level(board::pins::tft_dc, 1);
    return HAL::SPI::transmit(device_, data, length);
}

esp_err_t Display::setAddressWindow(const uint16_t x, const uint16_t y,
                                    const uint16_t width, const uint16_t height)
{
    if (width == 0 || height == 0 || x >= WIDTH || y >= HEIGHT)
    {
        return ESP_ERR_INVALID_ARG;
    }
    const uint16_t x_end = std::min<uint16_t>(WIDTH - 1, x + width - 1);
    const uint16_t y_end = std::min<uint16_t>(HEIGHT - 1, y + height - 1);
    const uint8_t columns[] = {static_cast<uint8_t>(x >> 8),
                               static_cast<uint8_t>(x & 0xFF),
                               static_cast<uint8_t>(x_end >> 8),
                               static_cast<uint8_t>(x_end & 0xFF)};
    const uint8_t pages[] = {static_cast<uint8_t>(y >> 8),
                             static_cast<uint8_t>(y & 0xFF),
                             static_cast<uint8_t>(y_end >> 8),
                             static_cast<uint8_t>(y_end & 0xFF)};

    esp_err_t result = writeCommand(ILI9341_CASET);
    if (result == ESP_OK) result = writeData(columns, sizeof(columns));
    if (result == ESP_OK) result = writeCommand(ILI9341_PASET);
    if (result == ESP_OK) result = writeData(pages, sizeof(pages));
    if (result == ESP_OK) result = writeCommand(ILI9341_RAMWR);
    return result;
}

esp_err_t Display::fillRect(uint16_t x, uint16_t y, uint16_t width,
                            uint16_t height, const uint16_t color)
{
    if (!initialized_ || x >= WIDTH || y >= HEIGHT)
    {
        return ESP_ERR_INVALID_STATE;
    }
    width = std::min<uint16_t>(width, WIDTH - x);
    height = std::min<uint16_t>(height, HEIGHT - y);
    esp_err_t result = setAddressWindow(x, y, width, height);
    if (result != ESP_OK)
    {
        return result;
    }

    uint8_t pixels[256] = {};
    for (size_t index = 0; index < sizeof(pixels); index += 2)
    {
        pixels[index] = static_cast<uint8_t>(color >> 8);
        pixels[index + 1] = static_cast<uint8_t>(color & 0xFF);
    }

    size_t remaining = static_cast<size_t>(width) * height;
    while (remaining > 0)
    {
        const size_t batch = std::min(remaining, sizeof(pixels) / 2);
        result = writeData(pixels, batch * 2);
        if (result != ESP_OK)
        {
            return result;
        }
        remaining -= batch;
    }
    return ESP_OK;
}

esp_err_t Display::drawLine(int x0, int y0, const int x1, const int y1,
                            const uint16_t color)
{
    const int dx = std::abs(x1 - x0);
    const int sx = x0 < x1 ? 1 : -1;
    const int dy = -std::abs(y1 - y0);
    const int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;
    while (true)
    {
        if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT)
        {
            const esp_err_t result = fillRect(static_cast<uint16_t>(x0),
                                              static_cast<uint16_t>(y0), 1, 1,
                                              color);
            if (result != ESP_OK) return result;
        }
        if (x0 == x1 && y0 == y1) break;
        const int twice_error = error * 2;
        if (twice_error >= dy) { error += dy; x0 += sx; }
        if (twice_error <= dx) { error += dx; y0 += sy; }
    }
    return ESP_OK;
}

esp_err_t Display::drawCircle(const int center_x, const int center_y,
                              int radius, const uint16_t color)
{
    int x = radius;
    int y = 0;
    int error = 0;
    while (x >= y)
    {
        const int points[][2] = {{center_x + x, center_y + y}, {center_x + y, center_y + x},
                                 {center_x - y, center_y + x}, {center_x - x, center_y + y},
                                 {center_x - x, center_y - y}, {center_x - y, center_y - x},
                                 {center_x + y, center_y - x}, {center_x + x, center_y - y}};
        for (const auto& point : points)
        {
            if (point[0] >= 0 && point[0] < WIDTH && point[1] >= 0 && point[1] < HEIGHT)
            {
                const esp_err_t result = fillRect(static_cast<uint16_t>(point[0]),
                                                  static_cast<uint16_t>(point[1]), 1, 1, color);
                if (result != ESP_OK) return result;
            }
        }
        if (error <= 0) { ++y; error += 2 * y + 1; }
        if (error > 0) { --x; error -= 2 * x + 1; }
    }
    return ESP_OK;
}

esp_err_t Display::drawText(const uint16_t x, const uint16_t y, const char* text,
                            const uint8_t scale, const uint16_t foreground,
                            const uint16_t background)
{
    if (scale == 0) return ESP_ERR_INVALID_ARG;
    uint16_t cursor_x = x;
    for (const char* character = text; *character != '\0'; ++character)
    {
        uint8_t rows[5];
        glyphRows(*character, rows);
        for (uint8_t row = 0; row < 5; ++row)
        {
            for (uint8_t column = 0; column < 3; ++column)
            {
                const bool on = (rows[row] & (1U << (2U - column))) != 0;
                const esp_err_t result = fillRect(cursor_x + column * scale,
                                                  y + row * scale, scale, scale,
                                                  on ? foreground : background);
                if (result != ESP_OK) return result;
            }
        }
        cursor_x += 4U * scale;
        if (cursor_x >= WIDTH) break;
    }
    return ESP_OK;
}

esp_err_t Display::renderTelemetry(const DisplayTelemetry& telemetry)
{
    if (!initialized_)
    {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t result = fillRect(0, 0, WIDTH, HEIGHT, BLACK);
    if (result != ESP_OK) return result;
    if ((result = fillRect(0, 0, WIDTH, 22, NAVY)) != ESP_OK) return result;
    if ((result = drawText(8, 4, "SURVIVABOARD", 2, CYAN, NAVY)) != ESP_OK) return result;

    char line[48] = {};
    if ((result = drawText(8, 32, telemetry.gps_fix ? "GPS FIX" : "GPS NO FIX", 2,
                           telemetry.gps_fix ? GREEN : ORANGE, BLACK)) != ESP_OK) return result;
    if (telemetry.gps_fix)
    {
        std::snprintf(line, sizeof(line), "LAT %.5f", telemetry.latitude);
        if ((result = drawText(8, 52, line, 2, WHITE, BLACK)) != ESP_OK) return result;
        std::snprintf(line, sizeof(line), "LON %.5f", telemetry.longitude);
        if ((result = drawText(8, 70, line, 2, WHITE, BLACK)) != ESP_OK) return result;
    }
    else if ((result = drawText(8, 52, "WAITING FOR SKY", 2, GRAY, BLACK)) != ESP_OK)
    {
        return result;
    }

    if ((result = drawText(208, 30, "HEADING", 2, YELLOW, BLACK)) != ESP_OK) return result;
    if (telemetry.heading_valid)
    {
        std::snprintf(line, sizeof(line), "%.0f %s", telemetry.heading_degrees,
                      cardinalDirection(telemetry.heading_degrees));
        if ((result = drawText(216, 50, line, 2, WHITE, BLACK)) != ESP_OK) return result;
        if (!telemetry.heading_calibrated &&
            (result = drawText(232, 70, "MAG UNCAL", 1, ORANGE, BLACK)) != ESP_OK)
        {
            return result;
        }
        constexpr int compass_x = 266;
        constexpr int compass_y = 105;
        constexpr int compass_radius = 35;
        if ((result = drawCircle(compass_x, compass_y, compass_radius, CYAN)) != ESP_OK) return result;
        if ((result = drawText(compass_x - 3, compass_y - compass_radius - 12, "N", 2, RED, BLACK)) != ESP_OK) return result;
        const float radians = (telemetry.heading_degrees - 90.0F) *
                              3.14159265F / 180.0F;
        const int end_x = compass_x + static_cast<int>(std::cos(radians) * (compass_radius - 5));
        const int end_y = compass_y + static_cast<int>(std::sin(radians) * (compass_radius - 5));
        if ((result = drawLine(compass_x, compass_y, end_x, end_y, RED)) != ESP_OK) return result;
    }
    else if ((result = drawText(214, 54, "UNCAL", 2, ORANGE, BLACK)) != ESP_OK)
    {
        return result;
    }

    if ((result = fillRect(0, 132, WIDTH, 2, BLUE)) != ESP_OK) return result;
    if ((result = drawText(8, 142, "ENVIRONMENT", 2, CYAN, BLACK)) != ESP_OK) return result;
    if (!telemetry.environment_valid)
    {
        return drawText(8, 164, "BME688 WAITING", 2, GRAY, BLACK);
    }

    std::snprintf(line, sizeof(line), "TEMP %.1f C  HUM %.1f PCT",
                  telemetry.temperature_c, telemetry.humidity_percent);
    if ((result = drawText(8, 164, line, 2, WHITE, BLACK)) != ESP_OK) return result;
    std::snprintf(line, sizeof(line), "PRESS %.1f HPA", telemetry.pressure_hpa);
    if ((result = drawText(8, 184, line, 2, WHITE, BLACK)) != ESP_OK) return result;
    if (telemetry.gas_valid)
    {
        std::snprintf(line, sizeof(line), "GAS %.0f OHM", telemetry.gas_resistance_ohms);
    }
    else
    {
        std::snprintf(line, sizeof(line), "GAS WARMING");
    }
    if ((result = drawText(8, 204, line, 2, telemetry.gas_valid ? GREEN : ORANGE, BLACK)) != ESP_OK) return result;

    if (telemetry.iaq_valid)
    {
        std::snprintf(line, sizeof(line), "IAQ %.0f", telemetry.iaq);
        return drawText(204, 204, line, 2, GREEN, BLACK);
    }
    return drawText(188, 204, "IAQ BSEC REQD", 2, ORANGE, BLACK);
}
