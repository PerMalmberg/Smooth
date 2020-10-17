#pragma once

#include "esp_eth_com.h"
#include "esp_eth_mac.h"
#include "esp_eth_phy.h"
#include <esp_err.h>

#define CONFIG_SMOOTH_ETH_PHY_MOCK 1
#define CONFIG_SMOOTH_USE_INTERNAL_ETHERNET 1
#define CONFIG_SMOOTH_ETH_MDC_GPIO 23
#define CONFIG_SMOOTH_ETH_MDIO_GPIO 18
#define CONFIG_SMOOTH_ETH_PHY_RST_GPIO 5
#define CONFIG_SMOOTH_ETH_PHY_ADDR 0

typedef void* esp_eth_handle_t;

typedef struct {
    esp_eth_mac_t* mac;
    esp_eth_phy_t* phy;
    uint32_t check_link_period_ms;
    esp_err_t (*stack_input)(esp_eth_handle_t eth_handle, uint8_t* buffer, uint32_t length, void* priv);
    esp_err_t (*on_lowlevel_init_done)(esp_eth_handle_t eth_handle);
    esp_err_t (*on_lowlevel_deinit_done)(esp_eth_handle_t eth_handle);
} esp_eth_config_t;

#define ESP_NETIF_DEFAULT_ETH() {}

#define ETH_DEFAULT_CONFIG(emac, ephy)   \
    {                                    \
        .mac = emac,                     \
        .phy = ephy,                     \
        .check_link_period_ms = 2000,    \
        .stack_input = NULL,             \
        .on_lowlevel_init_done = NULL,   \
        .on_lowlevel_deinit_done = NULL, \
    }

inline esp_err_t esp_eth_driver_install(const esp_eth_config_t* /*config*/, esp_eth_handle_t* /*out_hdl*/){ return ESP_OK; };
inline esp_err_t esp_eth_driver_uninstall(esp_eth_handle_t /*hdl*/){ return ESP_OK; };
inline esp_err_t esp_eth_start(esp_eth_handle_t /*hdl*/){ return ESP_OK; };
inline esp_err_t esp_eth_stop(esp_eth_handle_t /*hdl*/){ return ESP_OK; };
inline esp_err_t esp_eth_update_input_path(
    esp_eth_handle_t /*hdl*/,
    esp_err_t (* /*stack_input*/)(esp_eth_handle_t hdl, uint8_t* buffer, uint32_t length, void* priv),     void* /*priv*/)
{
    return ESP_OK; 
};
inline esp_err_t esp_eth_transmit(esp_eth_handle_t /*hdl*/, void* /*buf*/, uint32_t /*length*/){ return ESP_OK; };
inline esp_err_t esp_eth_receive(esp_eth_handle_t /*hdl*/, uint8_t* /*buf*/, uint32_t* /*length*/){ return ESP_OK; };
inline esp_err_t esp_eth_ioctl(esp_eth_handle_t /*hdl*/, esp_eth_io_cmd_t /*cmd*/, void* /*data*/){ return ESP_OK; };
inline esp_err_t esp_eth_increase_reference(esp_eth_handle_t /*hdl*/){ return ESP_OK; };
inline esp_err_t esp_eth_decrease_reference(esp_eth_handle_t /*hdl*/){ return ESP_OK; };
