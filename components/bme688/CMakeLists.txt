idf_component_register(
    SRCS "bme688.cpp"
    INCLUDE_DIRS "include"
    REQUIRES freertos hal_i2c
)
