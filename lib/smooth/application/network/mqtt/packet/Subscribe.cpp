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

#include <smooth/application/network/mqtt/packet/Subscribe.h>
#include <smooth/core/util/advance_iterator.h>
#include <smooth/application/network/mqtt/packet/IPacketReceiver.h>

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
