#pragma once

#include <esp_netif_types.h>
#include <esp_netif_ip_addr.h>
#include <esp_err.h>

inline esp_err_t esp_netif_set_hostname(esp_netif_t */*esp_netif*/, const char */*hostname*/) { return ESP_OK; }

inline void esp_netif_destroy(esp_netif_t* /*esp_netif*/) {}

inline esp_err_t esp_netif_init(void) { return ESP_OK; }

inline esp_err_t esp_netif_deinit(void) { return ESP_OK; }

inline esp_err_t esp_netif_get_mac(esp_netif_t */*esp_netif*/, uint8_t /*mac*/[]){ return ESP_OK; };

inline esp_err_t esp_netif_attach(esp_netif_t */*esp_netif*/, esp_netif_iodriver_handle /*driver_handle*/){ return ESP_OK; };

inline esp_netif_t *esp_netif_new(const esp_netif_config_t */*esp_netif_config*/){ return nullptr; };