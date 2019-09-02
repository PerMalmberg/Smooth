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

#include "smooth/core/network/PacketReceiveBuffer.h"

namespace smooth::core::network::event
{
    /// Event sent when data is available to the application
    /// \tparam PacketType The type of packet that is available.
    template<typename Protocol, typename Packet = typename Protocol::packet_type>
    class DataAvailableEvent
    {
        public:
            DataAvailableEvent() = default;

            explicit DataAvailableEvent(IPacketReceiveBuffer<Protocol>* rx)
                    : rx(rx)
            {
            }

            /// Gets the available data
            /// \param target The instance that will be assigned the data.
            /// \return True if the data could be retrieved, otherwise false.
            bool get(Packet& target) const
            {
                bool res = false;

                if (rx)
                {
                    res = rx->get(target);
                }

                return res;
            }

        private:
            IPacketReceiveBuffer<Protocol>* rx = nullptr;
    };
}
