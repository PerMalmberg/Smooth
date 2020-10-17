#pragma once

#include <esp_eth.h>

inline void* esp_eth_new_netif_glue(esp_eth_handle_t /*eth_hdl*/){ return ESP_OK; }
inline esp_err_t esp_eth_del_netif_glue(void* /*glue*/){ return ESP_OK; }
inline esp_err_t esp_eth_set_default_handlers(void* /*esp_netif*/){ return ESP_OK; }
inline esp_err_t esp_eth_clear_default_handlers(void* /*esp_netif*/){ return ESP_OK; }