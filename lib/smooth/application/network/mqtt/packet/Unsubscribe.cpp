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

#include <smooth/application/network/mqtt/packet/Unsubscribe.h>
#include <smooth/core/util/advance_iterator.h>
#include <smooth/application/network/mqtt/packet/IPacketReceiver.h>

namespace smooth::application::network::mqtt::packet
{
    void Unsubscribe::visit(IPacketReceiver& receiver)
    {
        receiver.receive(*this);
    }

    void Unsubscribe::get_topics(std::vector<std::string>& topics) const
    {
        calculate_remaining_length_and_variable_header_offset();
        auto it = get_variable_header_start() + get_variable_header_length();

        bool end_reached = false;

        do
        {
            auto s = get_string(it);
            topics.push_back(s);
            // Move to next string, add two for the length bits
            end_reached = !core::util::advance(it, data.end(), s.length() + 2);
        }
        while (!end_reached && it != data.end());
    }
}
