#pragma once

#include "driver/spi_master.h"
#include "esp_err.h"

class LoRa
{
public:
    esp_err_t begin();
    void setReceiveMode();
    void setTransmitMode();

private:
    spi_device_handle_t device_ = nullptr;
};
