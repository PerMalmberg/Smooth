#include <esp_wifi.h>

esp_err_t esp_wifi_disconnect()
{
    return ESP_OK;
}

esp_err_t esp_wifi_stop()
{
    return ESP_OK;
}

esp_err_t esp_wifi_start()
{
    return ESP_OK;
}

esp_err_t esp_wifi_deinit()
{
    return ESP_OK;
}

wifi_init_config_t WIFI_INIT_CONFIG_DEFAULT()
{
    return wifi_init_config_t{};
}

esp_err_t esp_wifi_init(const wifi_init_config_t*/*config*/)
{
    return ESP_OK;
}

esp_err_t esp_wifi_set_mode(wifi_mode_t /*mode*/)
{
    return ESP_OK;
}

esp_err_t esp_wifi_set_storage(wifi_storage_t /*storage*/)
{
    return ESP_OK;
}

esp_err_t esp_wifi_set_config(wifi_interface_t /*interface*/, wifi_config_t*/*conf*/)
{
    return ESP_OK;
}

esp_err_t esp_wifi_connect()
{
    return ESP_OK;
}

esp_err_t esp_wifi_get_mac(wifi_interface_t /*ifx*/, uint8_t mac[6])
{
    for (std::size_t i = 0; i < 6; ++i)
    {
        mac[i] = static_cast<uint8_t>(i);
    }

    return ESP_OK;
}

esp_err_t esp_wifi_get_mode(wifi_mode_t * mode)
{
    *mode = WIFI_MODE_STA;
    return ESP_OK;
}
