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
#include <esp_wifi.h>
#include <smooth/core/ipc/IEventListener.h>

namespace smooth::core::network
{
    /// Wifi management class
    class Wifi
        : public smooth::core::ipc::IEventListener<system_event_t>
    {
        public:
            Wifi();

            Wifi(const Wifi&) = delete;

            Wifi(Wifi&&) = delete;

            Wifi& operator=(const Wifi&) = delete;

            Wifi& operator=(Wifi&&) = delete;

            ~Wifi() override;

            /// Sets the hostname
            /// \param name The name
            void set_host_name(const std::string& name);

            /// Sets the credentials for the Wifi network
            /// \param ssid The SSID
            /// \param password The password
            void set_ap_credentials(const std::string& ssid, const std::string& password);

            /// Enables, disables auto reconnect on loss of Wifi connection.
            /// \param auto_connect
            void set_auto_connect(bool auto_connect);

            /// Initiates the connection to the AP.
            void connect_to_ap();

            /// Returns a value indicating of currently connected to the access point.
            /// \return
            [[nodiscard]] bool is_connected_to_ap() const;

            /// Returns a value indicating if the required settings are set.
            /// \return true or false.
            [[nodiscard]] bool is_configured() const
            {
                return host_name.length() > 0 && ssid.length() > 0 && password.length() > 0;
            }

            /// Event response method
            /// \param event The event
            void event(const system_event_t& event) override;

            [[nodiscard]] std::string get_mac_address() const;

            /// Start providing an access point
            /// \param max_conn maximum number of clients to connect to this AP
            void start_softap(uint8_t max_conn = 1);

        private:
            void connect() const;

            bool auto_connect_to_ap = false;
            bool connected_to_ap = false;
            std::string host_name = "Smooth-Wifi";
            std::string ssid{};
            std::string password{};
    };
}
