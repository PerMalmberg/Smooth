#pragma once

#include "esp_eth_com.h"
#include <stdbool.h>

typedef struct esp_eth_mac_s esp_eth_mac_t;

struct esp_eth_mac_s {
    esp_err_t (*set_mediator)(esp_eth_mac_t* mac, esp_eth_mediator_t* eth);
    esp_err_t (*init)(esp_eth_mac_t* mac);
    esp_err_t (*deinit)(esp_eth_mac_t* mac);
    esp_err_t (*start)(esp_eth_mac_t* mac);
    esp_err_t (*stop)(esp_eth_mac_t* mac);
    esp_err_t (*transmit)(esp_eth_mac_t* mac, uint8_t* buf, uint32_t length);
    esp_err_t (*receive)(esp_eth_mac_t* mac, uint8_t* buf, uint32_t* length);
    esp_err_t (*read_phy_reg)(esp_eth_mac_t* mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t* reg_value);
    esp_err_t (*write_phy_reg)(esp_eth_mac_t* mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value);
    esp_err_t (*set_addr)(esp_eth_mac_t* mac, uint8_t* addr);
    esp_err_t (*get_addr)(esp_eth_mac_t* mac, uint8_t* addr);
    esp_err_t (*set_speed)(esp_eth_mac_t* mac, eth_speed_t speed);
    esp_err_t (*set_duplex)(esp_eth_mac_t* mac, eth_duplex_t duplex);
    esp_err_t (*set_link)(esp_eth_mac_t* mac, eth_link_t link);
    esp_err_t (*set_promiscuous)(esp_eth_mac_t* mac, bool enable);
    esp_err_t (*del)(esp_eth_mac_t* mac);
};

/**
* @brief Configuration of Ethernet MAC object
*
*/
typedef struct {
    uint32_t sw_reset_timeout_ms; /*!< Software reset timeout value (Unit: ms) */
    uint32_t rx_task_stack_size;  /*!< Stack size of the receive task */
    uint32_t rx_task_prio;        /*!< Priority of the receive task */
    int smi_mdc_gpio_num;         /*!< SMI MDC GPIO number */
    int smi_mdio_gpio_num;        /*!< SMI MDIO GPIO number */
    uint32_t flags;               /*!< Flags that specify extra capability for mac driver */
} eth_mac_config_t;

#define ETH_MAC_FLAG_WORK_WITH_CACHE_DISABLE (1 << 0) /*!< MAC driver can work when cache is disabled */
#define ETH_MAC_FLAG_PIN_TO_CORE (1 << 1)             /*!< Pin MAC task to the CPU core where driver installation happened */

/**
 * @brief Default configuration for Ethernet MAC object
 *
 */
#define ETH_MAC_DEFAULT_CONFIG()    \
    {                               \
        .sw_reset_timeout_ms = 100, \
        .rx_task_stack_size = 4096, \
        .rx_task_prio = 15,         \
        .smi_mdc_gpio_num = 23,     \
        .smi_mdio_gpio_num = 18,    \
        .flags = 0,                 \
    }

/**
* @brief Create ESP32 Ethernet MAC instance
*
* @param config: Ethernet MAC configuration
*
* @return
*      - instance: create MAC instance successfully
*      - NULL: create MAC instance failed because some error occurred
*/
inline esp_eth_mac_t*
esp_eth_mac_new_esp32(const eth_mac_config_t* /*config*/)
{
    return nullptr;
}
