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
