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

#include <chrono>
#include <smooth/core/ipc/TaskEventQueue.h>

namespace smooth::application::network::mqtt
{
    class Publication;

    class Subscription;

    class IMqttClient
    {
        public:
            virtual ~IMqttClient() = default;

            virtual const std::string& get_client_id() const = 0;

            virtual const std::chrono::seconds get_keep_alive() const = 0;

            virtual void start_reconnect() = 0;

            virtual void reconnect() = 0;

            virtual bool is_auto_reconnect() const = 0;

            virtual void disconnect() = 0;

            virtual void force_disconnect() = 0;

            virtual void set_keep_alive_timer(std::chrono::seconds interval) = 0;

            virtual bool send_packet(packet::MQTTPacket& packet) = 0;

            virtual Publication& get_publication() = 0;

            virtual Subscription& get_subscription() = 0;

            virtual core::ipc::TaskEventQueue<std::pair<std::string, std::vector<uint8_t>>>&
            get_application_queue() = 0;
    };

}