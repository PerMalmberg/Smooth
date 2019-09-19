#include <driver/gpio.h>
#include <esp_err.h>

esp_err_t gpio_isr_handler_add(gpio_num_t /*gpio_num*/, gpio_isr_t /*isr_handler*/, void* args)
{
    return ESP_OK;
}

esp_err_t gpio_isr_handler_remove(gpio_num_t gpio_num)
{
    return ESP_OK;
}

esp_err_t gpio_config(const gpio_config_t* /*pGPIOConfig*/)
{
    return ESP_OK;
}

esp_err_t gpio_set_level(gpio_num_t /*gpio_num*/, uint32_t /*level*/)
{
    return ESP_OK;
}

int gpio_get_level(gpio_num_t /*gpio_num*/)
{
    return 0;
}

esp_err_t gpio_set_pull_mode(gpio_num_t /*gpio_num*/, gpio_pull_mode_t /*pull*/)
{
    return ESP_OK;
}

esp_err_t gpio_pullup_en(gpio_num_t /*gpio_num*/)
{
    return ESP_OK;
}

esp_err_t gpio_install_isr_service(int /*intr_alloc_flags*/)
{
    return ESP_OK;
}

void gpio_uninstall_isr_service()
{
}

esp_err_t gpio_intr_enable(gpio_num_t /*gpio_num*/)
{
    return ESP_OK;
}

esp_err_t gpio_intr_disable(gpio_num_t /*gpio_num*/)
{
    return ESP_OK;
}
