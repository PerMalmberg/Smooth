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
        Wifi();
        Wifi(const Wifi&) = delete;
        virtual ~Wifi();

        void connect_to_ap(const std::string& local_host_name,
                           const std::string& ssid,
                           const std::string& password,
                           bool enable_auto_connect);

        bool is_connected_to_ap() const;

        void system_event(Application& app, system_event_t& event) override;
    private:
        void connect();
        bool auto_connect_to_ap = false;
        bool connected_to_ap = false;
        std::string host_name;
};

