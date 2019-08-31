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

#include <vector>
#include <chrono>
#include <mutex>
#include <smooth/core/timer/ElapsedTime.h>
#include <smooth/application/network/mqtt/packet/PubAck.h>
#include <smooth/application/network/mqtt/packet/PubComp.h>
#include <smooth/application/network/mqtt/packet/Publish.h>
#include <smooth/application/network/mqtt/packet/PubRec.h>
#include <smooth/application/network/mqtt/IMqttClient.h>
#include <smooth/application/network/mqtt/InFlight.h>

namespace smooth::application::network::mqtt
{
    class Publication
    {
        public:
            Publication();

            bool
            publish(const std::string& topic, const uint8_t* data, int length, mqtt::QoS qos, bool retain);

            void publish_next(IMqttClient& mqtt);

            void handle_disconnect();

            void resend_outstanding_control_packet(IMqttClient& mqtt, bool clean_session);

            void receive(packet::PubAck& pub_ack, IMqttClient& mqtt);

            void receive(packet::PubRec& pub_rec, IMqttClient& mqtt);

            void receive(packet::PubComp& pub_rel, IMqttClient& mqtt);

        private:
            std::vector<InFlight<packet::Publish>> in_progress{};
            std::mutex guard{};
    };
}
