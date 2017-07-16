//
// Created by permal on 6/24/17.
//

#pragma once

#include <string>
#include <esp_wifi.h>
#include <smooth/ipc/IEventListener.h>

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

                void connect_to_ap(const std::string& local_host_name,
                                   const std::string& ssid,
                                   const std::string& password,
                                   bool enable_auto_connect);

                bool is_connected_to_ap() const;

                void message(const system_event_t& msg) override;

            private:
                void connect();
                bool auto_connect_to_ap = false;
                bool connected_to_ap = false;
                std::string host_name;
        };
    }
}