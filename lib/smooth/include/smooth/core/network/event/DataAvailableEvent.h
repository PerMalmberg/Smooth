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
