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

#include "smooth/core/network/Wifi.h"
#include <cstring>
#include <sstream>
#include <esp_wifi_types.h>
#include <esp_netif.h>
#include <esp_event.h>
#include "smooth/core/network/NetworkStatus.h"
#include "smooth/core/ipc/Publisher.h"
#include "smooth/core/util/copy_min_to_buffer.h"
#include "smooth/core/logging/log.h"

#ifdef ESP_PLATFORM
#include "sdkconfig.h"
static_assert(CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE >= 3072,
"Need enough stack to be able to log in the event loop callback.");
#endif

using namespace smooth::core::util;
using namespace smooth::core;

namespace smooth::core::network
{
    struct esp_ip4_addr Wifi::ip = { 0 };

    Wifi::Wifi()
    {
        esp_netif_init();
        esp_event_handler_instance_register(WIFI_EVENT,
                                           ESP_EVENT_ANY_ID,
                                           &Wifi::wifi_event_callback,
                                           this,
                                           &instance_wifi_event);

        esp_event_handler_instance_register(IP_EVENT,
                                           ESP_EVENT_ANY_ID,
                                           &Wifi::wifi_event_callback,
                                           this,
                                           &instance_ip_event);
    }

    Wifi::~Wifi()
    {
        esp_event_handler_instance_unregister(IP_EVENT, ESP_EVENT_ANY_ID, instance_ip_event);
        esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_wifi_event);
        esp_wifi_disconnect();
        esp_wifi_stop();
        esp_wifi_deinit();
        esp_netif_deinit();
    }

    void Wifi::set_host_name(const std::string& name)
    {
        host_name = name;
    }

    void Wifi::set_ap_credentials(const std::string& wifi_ssid, const std::string& wifi_password)
    {
        this->ssid = wifi_ssid;
        this->password = wifi_password;
    }

    void Wifi::set_auto_connect(bool auto_connect)
    {
        auto_connect_to_ap = auto_connect;
    }

    void Wifi::connect_to_ap()
    {
        // Prepare to connect to the provided SSID and password
        wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
        esp_wifi_init(&init);
        esp_wifi_set_mode(WIFI_MODE_STA);

        wifi_config_t config;
        memset(&config, 0, sizeof(config));
        copy_min_to_buffer(ssid.begin(), ssid.length(), config.sta.ssid);
        copy_min_to_buffer(password.begin(), password.length(), config.sta.password);

        config.sta.bssid_set = false;

        // Store Wifi settings in RAM - it is the applications responsibility to store settings.
        esp_wifi_set_storage(WIFI_STORAGE_RAM);
        esp_wifi_set_config(WIFI_IF_STA, &config);

        close_if();
        interface = esp_netif_create_default_wifi_sta();

        connect();
    }

    void Wifi::connect() const
    {
#ifdef ESP_PLATFORM
        esp_wifi_start();
        esp_wifi_connect();
#else

        // Assume network is available when running under POSIX system.
        publish_status(true, true);
#endif
    }

    bool Wifi::is_connected_to_ap() const
    {
        return connected_to_ap;
    }

    void Wifi::wifi_event_callback(void* event_handler_arg,
                                   esp_event_base_t event_base,
                                   int32_t event_id,
                                   void* event_data)
    {
        // Note: be very careful with what you do in this method - it runs under the event task
        // (sys_evt) with a very small default stack.
        Wifi* wifi = reinterpret_cast<Wifi*>(event_handler_arg);

        if (event_base == WIFI_EVENT)
        {
            if (event_id == WIFI_EVENT_STA_START)
            {
                esp_netif_set_hostname(wifi->interface, wifi->host_name.c_str());
            }
            else if (event_id == WIFI_EVENT_STA_CONNECTED)
            {
                wifi->connected_to_ap = true;
            }
            else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
            {
                wifi->ip.addr = 0;
                wifi->connected_to_ap = false;
                publish_status(wifi->connected_to_ap, true);

                if (wifi->auto_connect_to_ap)
                {
                    esp_wifi_stop();
                    wifi->connect();
                }
            }
            else if (event_id == WIFI_EVENT_AP_START)
            {
                wifi->ip.addr = 0xC0A80401; // 192.168.4.1
                publish_status(true, true);
            }
            else if (event_id == WIFI_EVENT_AP_STOP)
            {
                wifi->ip.addr = 0;
                Log::info("SoftAP", "AP stopped");
                publish_status(false, true);
            }
            else if (event_id == WIFI_EVENT_AP_STACONNECTED)
            {
                auto data = reinterpret_cast<wifi_event_ap_staconnected_t*>(event_data);
                Log::info("SoftAP", "Station connected. MAC: {}:{}:{}:{}:{}:{} join, AID={}",
                                         data->mac[0],
                                         data->mac[1],
                                         data->mac[2],
                                         data->mac[3],
                                         data->mac[4],
                                         data->mac[5],
                                         data->aid);
            }
            else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
            {
                auto data = reinterpret_cast<wifi_event_ap_stadisconnected_t*>(event_data);

                Log::info("SoftAP", "Station disconnected. MAC: {}:{}:{}:{}:{}:{} join, AID={}",
                                         data->mac[0],
                                         data->mac[1],
                                         data->mac[2],
                                         data->mac[3],
                                         data->mac[4],
                                         data->mac[5],
                                         data->aid);
            }
        }
        else if (event_base == IP_EVENT)
        {
            if (event_id == IP_EVENT_STA_GOT_IP
                || event_id == IP_EVENT_GOT_IP6
                || event_id == IP_EVENT_ETH_GOT_IP)
            {
                auto ip_changed = event_id == IP_EVENT_STA_GOT_IP ?
                                  reinterpret_cast<ip_event_got_ip_t*>(event_data)->ip_changed : true;
                publish_status(true, ip_changed);
                wifi->ip.addr = reinterpret_cast<ip_event_got_ip_t*>(event_data)->ip_info.ip.addr;
            }
            else if (event_id == IP_EVENT_STA_LOST_IP)
            {
                wifi->ip.addr = 0;
                publish_status(false, true);
            }
        }
    }

    void Wifi::close_if()
    {
        if (interface)
        {
            esp_netif_destroy(interface);
            interface = nullptr;
        }
    }

    std::string Wifi::get_mac_address()
    {
        std::stringstream mac;

        std::array<uint8_t, 6> m;
        bool ret = get_local_mac_address(m);

        if (ret)
        {
            for (const auto& v : m)
            {
                if (mac.tellp() > 0)
                {
                    mac << "_";
                }

                mac << std::hex << static_cast<int>(v);
            }
        }

        return mac.str();
    }

    bool Wifi::get_local_mac_address(std::array<uint8_t, 6>& m)
    {
        wifi_mode_t mode;
        esp_err_t err = esp_wifi_get_mode(&mode);

        if (err == ESP_OK)
        {
            if (mode == WIFI_MODE_STA)
            {
                err = esp_wifi_get_mac(WIFI_IF_STA, m.data());
            }
            else if (mode == WIFI_MODE_AP)
            {
                err = esp_wifi_get_mac(WIFI_IF_AP, m.data());
            }
            else
            {
                err = ESP_FAIL;
            }
        }

        if (err != ESP_OK)
        {
            Log::error("Wifi", "get_local_mac_address(): {}", esp_err_to_name(err));
        }

        return err == ESP_OK;
    }

    // attention: access to this function might have a threading issue.
    // It should be called from the main thread only!
    uint32_t Wifi::get_local_ip()
    {
        return ip.addr;
    }

    void Wifi::start_softap(uint8_t max_conn)
    {
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        esp_wifi_init(&cfg);

        wifi_config_t config{};

        copy_min_to_buffer(ssid.begin(), ssid.length(), config.ap.ssid);
        copy_min_to_buffer(password.begin(), password.length(), config.ap.password);

        config.ap.max_connection = max_conn;
        config.ap.authmode = password.empty() ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA_WPA2_PSK;

        close_if();
        interface = esp_netif_create_default_wifi_ap();

        esp_wifi_set_mode(WIFI_MODE_AP);
        esp_wifi_set_config(ESP_IF_WIFI_AP, &config);
        esp_wifi_start();

        Log::info("SoftAP", "SSID: {}; Auth {}", ssid, (password.empty() ? "Open" : "WPA2/PSK"));

#ifndef ESP_PLATFORM

        // Assume network is available when running under POSIX system.
        publish_status(true, true);
#endif
    }

    void Wifi::publish_status(bool connected, bool ip_changed)
    {
        network::NetworkStatus status(connected
                                      ? network::NetworkEvent::GOT_IP : network::NetworkEvent::DISCONNECTED,
                                      ip_changed);
        core::ipc::Publisher<network::NetworkStatus>::publish(status);
    }
}
