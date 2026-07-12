idf_component_register(
    SRCS "hal_uart.cpp"
    INCLUDE_DIRS "include"
    REQUIRES board_config esp_driver_uart
)
