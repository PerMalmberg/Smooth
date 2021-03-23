#pragma once

#include <ip4_addr.h>

typedef ip4_addr_t ip_addr_t;

typedef enum
{
    TCPIP_ADAPTER_IF_STA = 0,     /**< Wi-Fi STA (station) interface */
    TCPIP_ADAPTER_IF_AP,          /**< Wi-Fi soft-AP interface */
    TCPIP_ADAPTER_IF_ETH,         /**< Ethernet interface */
    TCPIP_ADAPTER_IF_TEST,        /**< tcpip stack test interface */
    TCPIP_ADAPTER_IF_MAX
} tcpip_adapter_if_t;

/** @brief legacy ip_info type
 */
typedef struct {
    ip4_addr_t ip;      /**< Interface IPV4 address */
    ip4_addr_t netmask; /**< Interface IPV4 netmask */
    ip4_addr_t gw;      /**< Interface IPV4 gateway address */
} tcpip_adapter_ip_info_t;
