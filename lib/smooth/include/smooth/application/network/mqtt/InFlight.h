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

#include <vector>
#include <chrono>
#include "smooth/core/timer/ElapsedTime.h"
#include "smooth/application/network/mqtt/packet/PubAck.h"
#include "smooth/application/network/mqtt/packet/PubComp.h"
#include "smooth/application/network/mqtt/packet/Publish.h"
#include "smooth/application/network/mqtt/packet/PubRec.h"
#include "smooth/application/network/mqtt/IMqttClient.h"

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
