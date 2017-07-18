//
// Created by permal on 6/24/17.
//

#pragma once

#include <string>
#include <esp_wifi.h>
#include <smooth/ipc/IEventListener.h>

#undef connect

namespace smooth
{
    namespace network
    {
        class Wifi
                : public smooth::ipc::IEventListener<system_event_t>
        {
            public:
                Wifi();
                Wifi(const Wifi&) = delete;
                virtual ~Wifi();

                void set_host_name(const std::string& name);
                void set_ap_credentials(const std::string& ssid, const std::string& password);
                void set_auto_connect(bool auto_connect);


                void connect_to_ap();

                bool is_connected_to_ap() const;

                bool is_configured() const
                {
                    return host_name.length() > 0 && ssid.length() > 0 && password.length() > 0;
                }

                void message(const system_event_t& msg) override;

            private:
                void connect();
                bool auto_connect_to_ap = false;
                bool connected_to_ap = false;
                std::string host_name = "Smooth-Wifi";
                std::string ssid;
                std::string password;
        };
    }
}