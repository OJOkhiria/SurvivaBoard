int app_main()
{
    init_power();
    init_display();
    init_i2c();
    init_spi();
    init_uart();

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}