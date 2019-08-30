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

#include <string>
#include <array>
#include <smooth/core/ipc/IEventListener.h>

namespace smooth::core::network
{
    /// Wifi management class
    class Wifi
    {
        public:
            Wifi() = default;

            Wifi(const Wifi&) = delete;

            virtual ~Wifi() = default;

            /// Sets the hostname
            /// \param name The name
            void set_host_name(const std::string& /*name*/) {}

            /// Sets the credentials for the Wifi network
            /// \param ssid The SSID
            /// \param password The password
            void set_ap_credentials(const std::string& ssid, const std::string& password)
            {
                this->ssid = ssid;
                this->password = password;
            }

            /// Enables, disables auto reconnect on loss of Wifi connection.
            /// \param auto_connect
            void set_auto_connect(bool /*auto_connect*/) {}

            /// Initiates the connection to the AP.
            void connect_to_ap() {}

            /// Returns a value indicating of currently connected to the access point.
            /// \return
            bool is_connected_to_ap() const { return true; }

            /// Returns a value indicating if the required settings are set.
            /// \return true or false.
            bool is_configured() const
            {
                return host_name.length() > 0 && ssid.length() > 0 && password.length() > 0;
            }

            std::string get_mac_address() { return "11:22:33:44:55:66"; }

            void start_softap(uint8_t /*max_conn = 1*/) {}
        private:
            std::string host_name = "Smooth-Wifi";
            std::string ssid;
            std::string password;
    };
}
