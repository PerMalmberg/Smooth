//
// Created by permal on 6/24/17.
//

#include <smooth/core/network/Wifi.h>
#include <cstring>
#include <esp_log.h>
#include <esp_wifi_types.h>
#include <tcpip_adapter.h>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            Wifi::Wifi()
            {
                tcpip_adapter_init();
            }

            Wifi::~Wifi()
            {
                esp_wifi_disconnect();
                esp_wifi_stop();
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
}