#pragma once

#include <esp_wifi_types.h>
#include <tcpip_adapter.h>

/** System event types enumeration */
typedef enum
{
    SYSTEM_EVENT_WIFI_READY = 0,           /*!< ESP32 WiFi ready */
    SYSTEM_EVENT_SCAN_DONE,                /*!< ESP32 finish scanning AP */
    SYSTEM_EVENT_STA_START,                /*!< ESP32 station start */
    SYSTEM_EVENT_STA_STOP,                 /*!< ESP32 station stop */
    SYSTEM_EVENT_STA_CONNECTED,            /*!< ESP32 station connected to AP */
    SYSTEM_EVENT_STA_DISCONNECTED,         /*!< ESP32 station disconnected from AP */
    SYSTEM_EVENT_STA_AUTHMODE_CHANGE,      /*!< the auth mode of AP connected by ESP32 station changed */
    SYSTEM_EVENT_STA_GOT_IP,               /*!< ESP32 station got IP from connected AP */
    SYSTEM_EVENT_STA_LOST_IP,              /*!< ESP32 station lost IP and the IP is reset to 0 */
    SYSTEM_EVENT_STA_WPS_ER_SUCCESS,       /*!< ESP32 station wps succeeds in enrollee mode */
    SYSTEM_EVENT_STA_WPS_ER_FAILED,        /*!< ESP32 station wps fails in enrollee mode */
    SYSTEM_EVENT_STA_WPS_ER_TIMEOUT,       /*!< ESP32 station wps timeout in enrollee mode */
    SYSTEM_EVENT_STA_WPS_ER_PIN,           /*!< ESP32 station wps pin code in enrollee mode */
    SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP,   /*!< ESP32 station wps overlap in enrollee mode */
    SYSTEM_EVENT_AP_START,                 /*!< ESP32 soft-AP start */
    SYSTEM_EVENT_AP_STOP,                  /*!< ESP32 soft-AP stop */
    SYSTEM_EVENT_AP_STACONNECTED,          /*!< a station connected to ESP32 soft-AP */
    SYSTEM_EVENT_AP_STADISCONNECTED,       /*!< a station disconnected from ESP32 soft-AP */
    SYSTEM_EVENT_AP_STAIPASSIGNED,         /*!< ESP32 soft-AP assign an IP to a connected station */
    SYSTEM_EVENT_AP_PROBEREQRECVED,        /*!< Receive probe request packet in soft-AP interface */
    SYSTEM_EVENT_GOT_IP6,                  /*!< ESP32 station or ap or ethernet interface v6IP addr is preferred */
    SYSTEM_EVENT_ETH_START,                /*!< ESP32 ethernet start */
    SYSTEM_EVENT_ETH_STOP,                 /*!< ESP32 ethernet stop */
    SYSTEM_EVENT_ETH_CONNECTED,            /*!< ESP32 ethernet phy link up */
    SYSTEM_EVENT_ETH_DISCONNECTED,         /*!< ESP32 ethernet phy link down */
    SYSTEM_EVENT_ETH_GOT_IP,               /*!< ESP32 ethernet got IP from connected AP */
    SYSTEM_EVENT_MAX                       /*!< Number of members in this enum */
} system_event_id_t;

/* add this macro define for compatible with old IDF version */
#ifndef SYSTEM_EVENT_AP_STA_GOT_IP6
#define SYSTEM_EVENT_AP_STA_GOT_IP6 SYSTEM_EVENT_GOT_IP6
#endif

/** Argument structure of SYSTEM_EVENT_STA_WPS_ER_FAILED event */
typedef wifi_event_sta_wps_fail_reason_t system_event_sta_wps_fail_reason_t;

/** Argument structure of SYSTEM_EVENT_SCAN_DONE event */
typedef wifi_event_sta_scan_done_t system_event_sta_scan_done_t;

/** Argument structure of SYSTEM_EVENT_STA_CONNECTED event */
typedef wifi_event_sta_connected_t system_event_sta_connected_t;

/** Argument structure of SYSTEM_EVENT_STA_DISCONNECTED event */
typedef wifi_event_sta_disconnected_t system_event_sta_disconnected_t;

/** Argument structure of SYSTEM_EVENT_STA_AUTHMODE_CHANGE event */
typedef wifi_event_sta_authmode_change_t system_event_sta_authmode_change_t;

/** Argument structure of SYSTEM_EVENT_STA_WPS_ER_PIN event */
typedef wifi_event_sta_wps_er_pin_t system_event_sta_wps_er_pin_t;

/** Argument structure of  event */
typedef wifi_event_ap_staconnected_t system_event_ap_staconnected_t;

/** Argument structure of  event */
typedef wifi_event_ap_stadisconnected_t system_event_ap_stadisconnected_t;

/** Argument structure of  event */
typedef wifi_event_ap_probe_req_rx_t system_event_ap_probe_req_rx_t;

/** Argument structure of  event */
typedef ip_event_ap_staipassigned_t system_event_ap_staipassigned_t;

/** Argument structure of  event */
typedef ip_event_got_ip_t system_event_sta_got_ip_t;

/** Argument structure of  event */
typedef ip_event_got_ip6_t system_event_got_ip6_t;

/** Union of all possible system_event argument structures */
typedef union {
    system_event_sta_connected_t connected;                        /*!< ESP32 station connected to AP */
    system_event_sta_disconnected_t disconnected;                  /*!< ESP32 station disconnected to AP */
    system_event_sta_scan_done_t scan_done;                        /*!< ESP32 station scan (APs) done */
    system_event_sta_authmode_change_t auth_change;                /*!< the auth mode of AP ESP32 station connected to
                                                                      changed */
    system_event_sta_got_ip_t got_ip;                              /*!< ESP32 station got IP, first time got IP or when
                                                                      IP is changed */
    system_event_sta_wps_er_pin_t sta_er_pin;                      /*!< ESP32 station WPS enrollee mode PIN code
                                                                      received */
    system_event_sta_wps_fail_reason_t sta_er_fail_reason;         /*!< ESP32 station WPS enrollee mode failed reason
                                                                      code received */
    system_event_ap_staconnected_t sta_connected;                  /*!< a station connected to ESP32 soft-AP */
    system_event_ap_stadisconnected_t sta_disconnected;            /*!< a station disconnected to ESP32 soft-AP */
    system_event_ap_probe_req_rx_t ap_probereqrecved;              /*!< ESP32 soft-AP receive probe request packet */
    system_event_ap_staipassigned_t ap_staipassigned;              /**< ESP32 soft-AP assign an IP to the station*/
    system_event_got_ip6_t got_ip6;                                /*!< ESP32 station　or ap or ethernet ipv6 addr state
                                                                      change to preferred */
} system_event_info_t;

/** Event, as a tagged enum */
typedef struct {
    system_event_id_t event_id;          /*!< event ID */
    system_event_info_t event_info;      /*!< event information */
} system_event_t;
