#pragma once

#include <esp_netif.h>

inline esp_netif_t* esp_netif_create_default_wifi_ap(void) { return nullptr; }

inline esp_netif_t* esp_netif_create_default_wifi_sta() { return nullptr; }