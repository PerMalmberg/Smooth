//
// Created by permal on 6/24/17.
//

#include <IDFApp/Wifi.h>
#include "esp_log.h"
#include <cstring>
#include <esp_wifi_types.h>

Wifi::Wifi(Application& app, const std::string& ssid, const std::string& password)
{
    // Prepare to connect to the provided SSID using the provided password
    wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_auto_connect(true));

    wifi_config_t config{};
    memset(&config, 0, sizeof(config));
    strncpy((char *) config.sta.ssid, ssid.c_str(), std::min(sizeof(config.sta.ssid), ssid.length()));
    strncpy((char *) config.sta.password, password.c_str(), std::min(sizeof(config.sta.password), password.length()));
    config.sta.bssid_set = false;

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &config));

    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}

Wifi::~Wifi()
{
    esp_wifi_disconnect();
    esp_wifi_stop();
}

bool Wifi::is_connected_to_ap() const
{
    return connected_to_ap;
}

void Wifi::system_event(Application& app, system_event_t& event)
{
    if (event.event_id == SYSTEM_EVENT_STA_CONNECTED)
    {
        connected_to_ap = true;
    } else if (event.event_id == SYSTEM_EVENT_STA_DISCONNECTED)
    {
        connected_to_ap = false;
    }
}