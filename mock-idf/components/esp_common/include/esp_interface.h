#pragma once

typedef enum
{
    ESP_IF_WIFI_STA = 0,     /**< ESP32 station interface */
    ESP_IF_WIFI_AP,          /**< ESP32 soft-AP interface */
    ESP_IF_ETH,              /**< ESP32 ethernet interface */
    ESP_IF_MAX
} esp_interface_t;
