idf_component_register(
    SRCS "gps.cpp"
    INCLUDE_DIRS "include"
    REQUIRES board_config esp_driver_gpio hal_uart
)
