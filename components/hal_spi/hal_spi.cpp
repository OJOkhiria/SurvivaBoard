#include "hal_spi.hpp"

#include "buses.hpp"
#include "pins.hpp"

namespace HAL::SPI
{
namespace
{
bool s_initialized = false;
}

esp_err_t init()
{
    if (s_initialized)
    {
        return ESP_OK;
    }

    spi_bus_config_t config = {};
    config.mosi_io_num = Pins::SPI_MOSI;
    config.miso_io_num = Pins::SPI_MISO;
    config.sclk_io_num = Pins::SPI_SCK;
    config.quadwp_io_num = GPIO_NUM_NC;
    config.quadhd_io_num = GPIO_NUM_NC;
    config.data4_io_num = GPIO_NUM_NC;
    config.data5_io_num = GPIO_NUM_NC;
    config.data6_io_num = GPIO_NUM_NC;
    config.data7_io_num = GPIO_NUM_NC;
    config.max_transfer_sz = 4096;
    config.flags = SPICOMMON_BUSFLAG_MASTER;
    config.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO;
    config.intr_flags = 0;
    config.data_io_default_level = false;

    const esp_err_t result = spi_bus_initialize(Buses::SPI, &config,
                                                 SPI_DMA_CH_AUTO);
    if (result == ESP_OK)
    {
        s_initialized = true;
    }
    return result;
}

esp_err_t addDevice(const int clock_hz, const int queue_size,
                    const gpio_num_t cs_pin, spi_device_handle_t* device)
{
    if (!s_initialized)
    {
        return ESP_ERR_INVALID_STATE;
    }

    spi_device_interface_config_t config = {};
    config.mode = 0;
    config.clock_source = SPI_CLK_SRC_DEFAULT;
    config.duty_cycle_pos = 128;
    config.clock_speed_hz = clock_hz;
    config.spics_io_num = cs_pin;
    config.queue_size = queue_size;
    config.sample_point = SPI_SAMPLING_POINT_PHASE_0;
    return spi_bus_add_device(Buses::SPI, &config, device);
}

esp_err_t transmit(spi_device_handle_t device, const void* tx_data,
                   const size_t length_bytes)
{
    spi_transaction_t transaction = {};
    transaction.length = length_bytes * 8;
    transaction.tx_buffer = tx_data;
    return spi_device_transmit(device, &transaction);
}
}  // namespace HAL::SPI
