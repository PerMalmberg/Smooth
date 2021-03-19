#pragma once

#include <esp_err.h>
#include <esp_netif_types.h>
#include <esp_netif_ip_addr.h>
#include <string>

//
// Note: tcpip_adapter legacy API has to be included by default to provide full compatibility
//  for applications that used tcpip_adapter API without explicit inclusion of tcpip_adapter.h
//
#if CONFIG_ESP_NETIF_TCPIP_ADAPTER_COMPATIBLE_LAYER
#define _ESP_NETIF_SUPPRESS_LEGACY_WARNING_
#include "tcpip_adapter.h"
#undef _ESP_NETIF_SUPPRESS_LEGACY_WARNING_
#endif // CONFIG_ESP_NETIF_TCPIP_ADAPTER_COMPAT

inline esp_err_t esp_netif_set_hostname(esp_netif_t*/*esp_netif*/, const char*/*hostname*/) { return ESP_OK; }

inline void esp_netif_destroy(esp_netif_t* /*esp_netif*/) {}

inline esp_err_t esp_netif_init(void) { return ESP_OK; }

inline esp_err_t esp_netif_deinit(void) { return ESP_OK; }

inline char* esp_ip4addr_ntoa(const esp_ip4_addr_t* /*addr*/, char* buf, int /*buflen*/)
{
    return buf;
}
