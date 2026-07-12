# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

idf_component_register(
    SRCS "main.cpp"
    REQUIRES
        bme688
        board_config
        display
        gps
        hal_i2c
        hal_spi
        hal_uart
        lora
        mmc5983ma
        esp_driver_gpio
)
