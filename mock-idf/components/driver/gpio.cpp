#include <driver/gpio.h>
#include <esp_err.h>

esp_err_t gpio_isr_handler_add(gpio_num_t /*gpio_num*/, gpio_isr_t /*isr_handler*/, void* args)
{
    return ESP_OK;
}

esp_err_t gpio_config(const gpio_config_t* /*pGPIOConfig*/)
{
    return ESP_OK;
}

int gpio_get_level(gpio_num_t /*gpio_num*/)
{
    return 0;
}
