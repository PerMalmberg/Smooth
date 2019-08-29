// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <smooth/core/network/Wifi.h>
#include <cstring>
#include <sstream>
#include <esp_log.h>
#include <esp_wifi_types.h>
#include <tcpip_adapter.h>
#include <smooth/core/network/NetworkStatus.h>
#include <smooth/core/ipc/Publisher.h>
#include <smooth/core/util/copy_n_to_buffer.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core;
using namespace smooth::core::util;

namespace smooth::core::network
{
    Wifi::Wifi()
    {
        tcpip_adapter_init();
    }

    Wifi::~Wifi()
    {
        esp_wifi_disconnect();
        esp_wifi_stop();
        esp_wifi_deinit();
    }

    void Wifi::set_host_name(const std::string& name)
    {
        host_name = name;
    }

    void Wifi::set_ap_credentials(const std::string& ssid, const std::string& password)
    {
        this->ssid = ssid;
        this->password = password;
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

        connect();
    }

    void Wifi::connect() const
    {
        esp_wifi_start();
        esp_wifi_connect();
    }

    bool Wifi::is_connected_to_ap() const
    {
        return connected_to_ap;
    }

    void Wifi::event(const system_event_t& event)
    {
        if (event.event_id == SYSTEM_EVENT_STA_START)
        {
            tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, host_name.c_str());
        }
        else if (event.event_id == SYSTEM_EVENT_STA_GOT_IP || event.event_id == SYSTEM_EVENT_AP_STA_GOT_IP6)
        {
            network::NetworkStatus status(network::NetworkEvent::GOT_IP, event.event_info.got_ip.ip_changed);
            core::ipc::Publisher<network::NetworkStatus>::publish(status);
        }
        else if (event.event_id == SYSTEM_EVENT_STA_CONNECTED)
        {
            connected_to_ap = true;
        }
        else if (event.event_id == SYSTEM_EVENT_STA_DISCONNECTED)
        {
            network::NetworkStatus status(network::NetworkEvent::DISCONNECTED, true);
            core::ipc::Publisher<network::NetworkStatus>::publish(status);

            connected_to_ap = false;
            if (auto_connect_to_ap)
            {
                esp_wifi_stop();
                connect();
            }
        }
        else if (event.event_id == SYSTEM_EVENT_AP_START)
        {
            network::NetworkStatus status(network::NetworkEvent::GOT_IP, event.event_info.got_ip.ip_changed);
            core::ipc::Publisher<network::NetworkStatus>::publish(status);
        }
        else if (event.event_id == SYSTEM_EVENT_AP_STACONNECTED)
        {
            auto& event_info = event.event_info;
            Log::info("SoftAP", Format("station MAC: {1}:{2}:{3}:{4}:{5}:{6} join, AID={7}",
                    Hex(event_info.sta_connected.mac[0]),
                    Hex(event_info.sta_connected.mac[1]),
                    Hex(event_info.sta_connected.mac[2]),
                    Hex(event_info.sta_connected.mac[3]),
                    Hex(event_info.sta_connected.mac[4]),
                    Hex(event_info.sta_connected.mac[5]),
                    UInt32(event_info.sta_connected.aid)));
        }
        else if (event.event_id == SYSTEM_EVENT_AP_STADISCONNECTED)
        {
            auto& event_info = event.event_info;
            Log::info("SoftAP", Format("station MAC: {1}:{2}:{3}:{4}:{5}:{6} join, AID={7}",
                    Hex(event_info.sta_connected.mac[0]),
                    Hex(event_info.sta_connected.mac[1]),
                    Hex(event_info.sta_connected.mac[2]),
                    Hex(event_info.sta_connected.mac[3]),
                    Hex(event_info.sta_connected.mac[4]),
                    Hex(event_info.sta_connected.mac[5]),
                    UInt32(event_info.sta_connected.aid)));
        }
        else if (event.event_id == SYSTEM_EVENT_AP_STOP)
        {
            Log::info("SoftAP", "AP stopped");
            network::NetworkStatus status(network::NetworkEvent::DISCONNECTED, true);
            core::ipc::Publisher<network::NetworkStatus>::publish(status);
        }
    }

    std::string Wifi::get_mac_address() const
    {
        std::stringstream mac;

        uint8_t m[6];
        if (esp_wifi_get_mac(WIFI_IF_STA, m) == ESP_OK)
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

    void Wifi::start_softap(uint8_t max_conn)
    {
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        esp_wifi_init(&cfg);

        wifi_config_t config{};

        copy_min_to_buffer(ssid.begin(), ssid.length(), config.ap.ssid);
        copy_min_to_buffer(password.begin(), password.length(), config.ap.password);

        config.ap.max_connection = max_conn;
        config.ap.authmode = password.empty() ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA_WPA2_PSK;

        esp_wifi_set_mode(WIFI_MODE_AP);
        esp_wifi_set_config(ESP_IF_WIFI_AP, &config);
        esp_wifi_start();

        Log::info("SoftAP", "SSID: " + ssid + "; Auth: " + (password.empty() ? "Open" : "WPA2/PSK"));
    }
}
