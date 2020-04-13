#pragma once

#include <esp_netif_types.h>
#include <esp_netif_ip_addr.h>

inline esp_err_t esp_netif_set_hostname(esp_netif_t */*esp_netif*/, const char */*hostname*/) { return ESP_OK; }

inline void esp_netif_destroy(esp_netif_t* /*esp_netif*/) {}

inline esp_err_t esp_netif_init(void) { return ESP_OK; }

inline esp_err_t esp_netif_deinit(void) { return ESP_OK; }
