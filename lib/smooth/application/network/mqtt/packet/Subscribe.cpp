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

#include "smooth/application/network/mqtt/packet/Subscribe.h"
#include "smooth/core/util/advance_iterator.h"
#include "smooth/application/network/mqtt/packet/IPacketReceiver.h"

namespace smooth::application::network::mqtt::packet
{
    void Subscribe::visit(IPacketReceiver& receiver)
    {
        receiver.receive(*this);
    }

    void Subscribe::get_topics(std::vector<std::pair<std::string, QoS>>& topics) const
    {
        calculate_remaining_length_and_variable_header_offset();
        auto it = get_variable_header_start() + get_variable_header_length();

        while (it != data.end())
        {
            auto s = get_string(it);

            // Move to QoS after string, add two for the length bits
            if (core::util::advance(it, data.end(), s.length() + 2))
            {
                auto qos = static_cast<QoS>(*it);
                topics.emplace_back(s, qos);

                // Move past QoS to next string
                it++;
            }
            else
            {
                // Uho, packet is malformed, skip rest.
                it = data.end();
            }
        }
    }
}
