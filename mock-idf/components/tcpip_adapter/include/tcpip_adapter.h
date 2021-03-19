#pragma once

#include <tcpip_adapter_types.h>
#include <esp_err.h>

/**
 * @brief tcpip adapter legacy init. It is used only to set the compatibility mode of esp-netif, which
 * will enable backward compatibility of esp-netif.
 */
void tcpip_adapter_init(void)  __attribute__((deprecated));

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_set_hostname
*/
esp_err_t tcpip_adapter_set_hostname(tcpip_adapter_if_t tcpip_if, const char* hostname);
