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

#pragma once

namespace smooth::core::network
{
    enum class NetworkEvent
    {
        GOT_IP,
        DISCONNECTED
    };

    class NetworkStatus
    {
        public:
            NetworkStatus(const NetworkEvent event, const bool ip_changed)
                    : event(event), ip_changed(ip_changed)
            {
            }

            NetworkStatus() = default;

            NetworkStatus(const NetworkStatus&) = default;

            NetworkStatus& operator=(const NetworkStatus&) = default;

            NetworkEvent get_event() const
            { return event; }

            bool get_ip_changed() const
            { return ip_changed; }
        private:
            NetworkEvent event{ NetworkEvent::DISCONNECTED };
            bool ip_changed{ false };
    };
}
