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
#include <smooth/core/timer/ElapsedTime.h>
#include <smooth/application/network/mqtt/packet/PubAck.h>
#include <smooth/application/network/mqtt/packet/PubComp.h>
#include <smooth/application/network/mqtt/packet/Publish.h>
#include <smooth/application/network/mqtt/packet/PubRec.h>
#include <smooth/application/network/mqtt/IMqttClient.h>

namespace smooth::application::network::mqtt
{
    template<typename T>
    class InFlight
    {
        public:
            explicit InFlight(T& p)
                    : p(p)
            {
            }

            T& get_packet()
            {
                return p;
            }

            PacketType get_waiting_for()
            {
                return waiting_for_packet;
            }

            void set_wait_packet(PacketType type)
            {
                waiting_for_packet = type;
            }

            void start_timer()
            {
                timer.start();
            }

            void stop_timer()
            {
                timer.stop();
            }

            void zero_timer()
            {
                timer.stop();
                timer.zero();
            }

            std::chrono::milliseconds get_elapsed_time()
            {
                return std::chrono::duration_cast<std::chrono::milliseconds>(timer.get_running_time());
            }

        private:
            T p{};
            PacketType waiting_for_packet = PacketType::Reserved;
            core::timer::ElapsedTime timer{};
    };
}
