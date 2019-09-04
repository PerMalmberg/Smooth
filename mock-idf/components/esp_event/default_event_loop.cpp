#include <default_event_loop.h>

const char* WIFI_EVENT = "wifi_event";
const char* IP_EVENT = "ip_event";
const char* ETH_EVENT = "eth_event";

esp_err_t esp_event_loop_create_default()
{
    return ESP_OK;
}

esp_err_t esp_event_loop_delete_default()
{
    return ESP_OK;
}

esp_err_t esp_event_handler_register(esp_event_base_t /*event_base*/, int32_t /*event_id*/,
                                     esp_event_handler_t /*event_handler*/, void* /*event_handler_arg*/)
{
    return ESP_OK;
}

esp_err_t esp_event_handler_unregister(esp_event_base_t /*event_base*/, int32_t /*event_id*/,
                                       esp_event_handler_t /*event_handler*/)
{
    return ESP_OK;
}
