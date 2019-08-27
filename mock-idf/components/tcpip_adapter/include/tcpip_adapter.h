#pragma once

#include <ip4_addr.h>
#include <ip6_addr.h>

/** Event structure for IP_EVENT_AP_STAIPASSIGNED event */
typedef struct {
    ip4_addr_t ip; /*!< IP address which was assigned to the station */
} ip_event_ap_staipassigned_t;

typedef struct {
    ip6_addr_t ip; /**< Interface IPV6 address */
} tcpip_adapter_ip6_info_t;

typedef enum {
    TCPIP_ADAPTER_IF_STA = 0,     /**< Wi-Fi STA (station) interface */
    TCPIP_ADAPTER_IF_AP,          /**< Wi-Fi soft-AP interface */
    TCPIP_ADAPTER_IF_ETH,         /**< Ethernet interface */
    TCPIP_ADAPTER_IF_TEST,        /**< tcpip stack test interface */
    TCPIP_ADAPTER_IF_MAX
} tcpip_adapter_if_t;

typedef struct {
    ip4_addr_t ip;      /**< Interface IPV4 address */
    ip4_addr_t netmask; /**< Interface IPV4 netmask */
    ip4_addr_t gw;      /**< Interface IPV4 gateway address */
} tcpip_adapter_ip_info_t;

typedef struct {
    tcpip_adapter_if_t if_index;        /*!< Interface for which the event is received */
    tcpip_adapter_ip_info_t ip_info;    /*!< IP address, netmask, gatway IP address */
    bool ip_changed;                    /*!< Whether the assigned IP has changed or not */
} ip_event_got_ip_t;

typedef struct {
    tcpip_adapter_if_t if_index;        /*!< Interface for which the event is received */
    tcpip_adapter_ip6_info_t ip6_info;  /*!< IPv6 address of the interface */
} ip_event_got_ip6_t;