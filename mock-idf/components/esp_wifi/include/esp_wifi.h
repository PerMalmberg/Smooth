#pragma once

#include <esp_event_legacy.h>
#include <esp_err.h>
#include <esp_netif_types.h>
#include <esp_wifi_default.h>

typedef struct
{
} wifi_init_config_t;

esp_err_t esp_wifi_disconnect();

esp_err_t esp_wifi_stop();

esp_err_t esp_wifi_start();

esp_err_t esp_wifi_deinit();

wifi_init_config_t WIFI_INIT_CONFIG_DEFAULT();

esp_err_t esp_wifi_init(const wifi_init_config_t* config);

esp_err_t esp_wifi_set_mode(wifi_mode_t mode);

esp_err_t esp_wifi_set_storage(wifi_storage_t storage);

esp_err_t esp_wifi_set_config(wifi_interface_t interface, wifi_config_t* conf);

esp_err_t esp_wifi_connect();

esp_err_t esp_wifi_get_mac(wifi_interface_t ifx, uint8_t mac[6]);

esp_err_t esp_wifi_get_mode(wifi_mode_t * mode);
