idf_component_register(
    SRCS "hal_spi.cpp"
    INCLUDE_DIRS "include"
    REQUIRES board_config esp_driver_spi
)
