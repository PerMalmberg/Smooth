//
// Created by permal on 6/24/17.
//

#pragma once

#include <string>
#include <esp_wifi.h>
#include <IDFApp/Application.h>
#include <IDFApp/SystemEventListener.h>

class Wifi
        : public SystemEventListener
{
    public:
        Wifi(Application& app, const std::string& ssid, const std::string& password);
        Wifi(const Wifi&) = delete;
        virtual ~Wifi();
        bool is_connected_to_ap() const;
        void system_event(Application& app, system_event_t& event) override;
    private:
        bool connected_to_ap = false;
};

