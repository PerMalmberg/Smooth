/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "smooth/core/network/Ethernet.h"
#include "smooth/core/ipc/Publisher.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/network/NetworkStatus.h"
#include "smooth/core/util/copy_min_to_buffer.h"
#include <cstring>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <esp_eth.h>
#include <esp_eth_netif_glue.h>
#include <esp_event.h>
#include <esp_netif.h>
#pragma GCC diagnostic pop

#ifdef ESP_PLATFORM
#include "sdkconfig.h"
static_assert(CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE >= 3072,
              "Need enough stack to be able to log in the event loop callback.");
#endif

using namespace smooth::core::util;
using namespace smooth::core;

namespace smooth::core::network {
    Ethernet::Ethernet(std::string&& name)
            : NetworkInterface(std::move(name))
    {
        esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
        interface = esp_netif_new(&cfg);
        esp_eth_set_default_handlers(interface);

        esp_event_handler_instance_register(ETH_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &Ethernet::eth_event_callback,
                                        this,
                                        &instance_eth_event);

        esp_event_handler_instance_register(IP_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &Ethernet::eth_event_callback,
                                        this,
                                        &instance_ip_event);

        eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
        eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
        phy_config.phy_addr = CONFIG_SMOOTH_ETH_PHY_ADDR;
        phy_config.reset_gpio_num = CONFIG_SMOOTH_ETH_PHY_RST_GPIO;
#if CONFIG_SMOOTH_USE_INTERNAL_ETHERNET
        mac_config.smi_mdc_gpio_num = CONFIG_SMOOTH_ETH_MDC_GPIO;
        mac_config.smi_mdio_gpio_num = CONFIG_SMOOTH_ETH_MDIO_GPIO;
        mac = esp_eth_mac_new_esp32(&mac_config);
#if CONFIG_SMOOTH_ETH_PHY_IP101
        phy = esp_eth_phy_new_ip101(&phy_config);
#elif CONFIG_SMOOTH_ETH_PHY_RTL8201
        phy = esp_eth_phy_new_rtl8201(&phy_config);
#elif CONFIG_SMOOTH_ETH_PHY_LAN8720
        phy = esp_eth_phy_new_lan8720(&phy_config);
#elif CONFIG_SMOOTH_ETH_PHY_LAN8742
        phy = esp_eth_phy_new_lan8742(&phy_config);
#elif CONFIG_SMOOTH_ETH_PHY_DP83848
        phy = esp_eth_phy_new_dp83848(&phy_config);
#elif CONFIG_SMOOTH_ETH_PHY_KSZ8041
        phy = esp_eth_phy_new_ksz8041(&phy_config);
#elif CONFIG_SMOOTH_ETH_PHY_MOCK
        phy = esp_eth_phy_new_mock(&phy_config);
#else
#error CONFIG_SMOOTH_ETH_PHY_type not set
#endif
#elif CONFIG_SMOOTH_USE_DM9051
        gpio_install_isr_service(0);
        spi_device_handle_t spi_handle = NULL;
        spi_bus_config_t buscfg = {
            .miso_io_num = CONFIG_SMOOTH_DM9051_MISO_GPIO,
            .mosi_io_num = CONFIG_SMOOTH_DM9051_MOSI_GPIO,
            .sclk_io_num = CONFIG_SMOOTH_DM9051_SCLK_GPIO,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
        };
        spi_bus_initialize(CONFIG_SMOOTH_DM9051_SPI_HOST, &buscfg, 1);

        if (err != ESP_OK)
        {
            return err;
        }

        spi_device_interface_config_t = devcfg = {
            .command_bits = 1,
            .address_bits = 7,
            .mode = 0,
            .clock_speed_hz = CONFIG_SMOOTH_DM9051_SPI_CLOCK_MHZ * 1000 * 1000,
            .spics_io_num = CONFIG_SMOOTH_DM9051_CS_GPIO,
            .queue_size = 20 };
        spi_bus_add_device(CONFIG_SMOOTH_DM9051_SPI_HOST, &devcfg, &spi_handle);

        if (err != ESP_OK)
        {
            return err;
        }

        /* dm9051 ethernet driver is based on spi driver */
        eth_dm9051_config_t dm9051_config = ETH_DM9051_DEFAULT_CONFIG(spi_handle);
        dm9051_config.int_gpio_num = CONFIG_SMOOTH_DM9051_INT_GPIO;
        mac = esp_eth_mac_new_dm9051(&dm9051_config, &mac_config);
        phy = esp_eth_phy_new_dm9051(&phy_config);
#else
#error CONFIG_SMOOTH_USE_INTERNAL_ETHERNET or CONFIG_SMOOTH_USE_DM9051 should be set
#endif
        esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
        esp_eth_driver_install(&config, &eth_handle);
    }

    Ethernet::~Ethernet()
    {
        esp_event_handler_instance_unregister(IP_EVENT, ESP_EVENT_ANY_ID, instance_ip_event);
        esp_event_handler_instance_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, instance_eth_event);

        if (eth_handle)
        {
            esp_eth_stop(eth_handle);
        }
    }

    void Ethernet::start()
    {
        apply_host_name();
        auto err = esp_netif_attach(interface, esp_eth_new_netif_glue(eth_handle));

        if (err == ESP_OK)
        {
            /* start Ethernet driver state machine */
            err = esp_eth_start(eth_handle);
        }

        if (err != ESP_OK)
        {
            Log::error(interface_name, "start", esp_err_to_name(err));
        }
    }

    bool Ethernet::is_connected() const
    {
        return connected;
    }

    void Ethernet::eth_event_callback(void* event_handler_arg,
                                      esp_event_base_t event_base,
                                      int32_t event_id,
                                      void* event_data)
    {
        // Note: be very careful with what you do in this method - it runs under the event task
        // (sys_evt) with a very small default stack.
        Ethernet* eth = reinterpret_cast<Ethernet*>(event_handler_arg);

        if (event_base == ETH_EVENT)
        {
            switch (event_id) {
            case ETHERNET_EVENT_START: {
            }
            break;
            case ETHERNET_EVENT_STOP: {
                eth->ip.addr = 0;
                publish_status(false, true);
            }
            break;
            case ETHERNET_EVENT_CONNECTED: {
                eth->connected = true;
            }
            break;
            case ETHERNET_EVENT_DISCONNECTED: {
                eth->ip.addr = 0;
                eth->connected = false;
                publish_status(eth->connected, true);
            }
            break;
            }
        }
        else if (event_base == IP_EVENT)
        {
            if (event_id == IP_EVENT_STA_GOT_IP
                || event_id == IP_EVENT_GOT_IP6
                || event_id == IP_EVENT_ETH_GOT_IP)
            {
                auto ip_changed = event_id ==
                                  IP_EVENT_STA_GOT_IP ? reinterpret_cast<ip_event_got_ip_t*>(event_data)->ip_changed :
                                  true;
                publish_status(true, ip_changed);
                eth->ip.addr = reinterpret_cast<ip_event_got_ip_t*>(event_data)->ip_info.ip.addr;
            }
            else if (event_id == IP_EVENT_STA_LOST_IP)
            {
                eth->ip.addr = 0;
                publish_status(false, true);
            }
        }
    }

    void Ethernet::publish_status(bool connected, bool ip_changed)
    {
        network::NetworkStatus status(connected
                                      ? network::NetworkEvent::GOT_IP
                                      : network::NetworkEvent::DISCONNECTED,
                                      ip_changed);
        core::ipc::Publisher<network::NetworkStatus>::publish(status);
    }
}
