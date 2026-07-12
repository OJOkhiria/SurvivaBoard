idf_component_register(
    SRCS "lora.cpp"
    INCLUDE_DIRS "include"
    REQUIRES board_config esp_driver_gpio hal_spi
)
