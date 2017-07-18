//
// Created by permal on 6/24/17.
//

#include <smooth/network/Wifi.h>
#include <cstring>
#include <esp_log.h>
#include <esp_wifi_types.h>
#include <tcpip_adapter.h>

namespace smooth
{
    namespace network
    {
        Wifi::Wifi()
                : host_name()
        {
            tcpip_adapter_init();
        }

        Wifi::~Wifi()
        {
            esp_wifi_disconnect();
            esp_wifi_stop();
        }

        void Wifi::connect_to_ap(const std::string& local_host_name,
                                 const std::string& ssid,
                                 const std::string& password,
                                 bool enable_auto_connect)
        {
            // Prepare to connect to the provided SSID and password
            wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
            esp_wifi_init(&init);
            esp_wifi_set_mode(WIFI_MODE_STA);

            //QQQ What effect does this setting actually have? esp_wifi_set_auto_connect(true);

            wifi_config_t config;
            memset(&config, 0, sizeof(config));
            strncpy((char*) config.sta.ssid, ssid.c_str(), std::min(sizeof(config.sta.ssid), ssid.length()));
            strncpy((char*) config.sta.password, password.c_str(),
                    std::min(sizeof(config.sta.password), password.length()));
            config.sta.bssid_set = false;

            // Store Wifi settings in RAM - it is the applications responsibility to store settings.
            esp_wifi_set_storage(WIFI_STORAGE_RAM);
            esp_wifi_set_config(WIFI_IF_STA, &config);

            auto_connect_to_ap = enable_auto_connect;
            host_name = local_host_name;

            connect();
        }

        void Wifi::connect()
        {
            esp_wifi_start();
            esp_wifi_connect();
        }

        bool Wifi::is_connected_to_ap() const
        {
            return connected_to_ap;
        }

        void Wifi::message(const system_event_t& msg)
        {
            if (msg.event_id == SYSTEM_EVENT_STA_START)
            {
                tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, host_name.c_str());
            }
            else if (msg.event_id == SYSTEM_EVENT_STA_CONNECTED)
            {
                connected_to_ap = true;
            }
            else if (msg.event_id == SYSTEM_EVENT_STA_DISCONNECTED)
            {
                connected_to_ap = false;
                if (auto_connect_to_ap)
                {
                    esp_wifi_stop();
                    connect();
                }
            }
        }
    }
}