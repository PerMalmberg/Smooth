#pragma once

#include <ip4_addr.h>
#include <ip6_addr.h>
#include <esp_err.h>
#include <esp_event_base.h>

/** Event structure for IP_EVENT_AP_STAIPASSIGNED event */
typedef struct {
    ip4_addr_t ip; /*!< IP address which was assigned to the station */
} ip_event_ap_staipassigned_t;

typedef struct {
    ip6_addr_t ip; /**< Interface IPV6 address */
} tcpip_adapter_ip6_info_t;

ESP_EVENT_DECLARE_BASE(IP_EVENT);

typedef enum
{
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

/** IP event declarations */
typedef enum
{
    IP_EVENT_STA_GOT_IP,               /*!< ESP32 station got IP from connected AP */
    IP_EVENT_STA_LOST_IP,              /*!< ESP32 station lost IP and the IP is reset to 0 */
    IP_EVENT_AP_STAIPASSIGNED,         /*!< ESP32 soft-AP assign an IP to a connected station */
    IP_EVENT_GOT_IP6,                  /*!< ESP32 station or ap or ethernet interface v6IP addr is preferred */
    IP_EVENT_ETH_GOT_IP,               /*!< ESP32 ethernet got IP from connected AP */
} ip_event_t;

void tcpip_adapter_init();

esp_err_t tcpip_adapter_set_hostname(tcpip_adapter_if_t tcpip_if, const char* hostname);
