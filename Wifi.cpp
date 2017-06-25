//
// Created by permal on 6/24/17.
//

#include "Wifi.h"
#include <esp_wifi.h>
#include "esp_log.h"
#include <cstring>
#include <esp_wifi_types.h>

Wifi::Wifi(const std::string &ssid, const std::string &password)
{
    wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&init);
    esp_wifi_set_mode( WIFI_MODE_STA );

    wifi_config_t config{};
    strncpy( (char*)config.sta.ssid, ssid.c_str(), std::min(sizeof(config.sta.ssid), ssid.length()));
    strncpy( (char*)config.sta.password, password.c_str(), std::min(sizeof(config.sta.password), password.length()));
    config.sta.bssid_set = false;

    esp_wifi_set_config(WIFI_IF_STA, &config );


    ESP_LOGV( "Wifi", "esp_wifi_start: %d", esp_wifi_start() );
    ESP_LOGV( "Wifi", "esp_wifi_connect: %d", esp_wifi_connect() );
}

Wifi::~Wifi() {
    esp_wifi_disconnect();
    esp_wifi_stop();
}

bool Wifi::is_connected() const {
    wifi_ap_record_t ap_info{};
    esp_err_t res = esp_wifi_sta_get_ap_info( &ap_info );

    bool is_connected = res == ESP_OK;

    return is_connected;
}