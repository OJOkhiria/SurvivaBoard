#pragma once

#include "driver/spi_master.h"
#include "esp_err.h"

class Display
{
public:
    esp_err_t begin();

private:
    spi_device_handle_t device_ = nullptr;
};
