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

#include <smooth/application/network/mqtt/packet/MQTTProtocol.h>

namespace smooth::application::network::mqtt::packet
{
    class ConnAck
            : public MQTTPacket
    {
        public:
            enum ReturnCode
            {
                ACCEPTED = 0,
                UNSUPPORTED_PROTOCOL_VERSION,
                REJECTED_IDENTIFIER,
                SERVER_UNAVAILABLE,
                BAD_CREDENTIALS,
                NOT_AUTHORIZED,
            };

            ConnAck() = default;

            explicit ConnAck(const MQTTPacket& packet)
                    : MQTTPacket(packet)
            {
            }

            bool is_session_present()
            {
                core::util::ByteSet b(*get_variable_header_start());
                return b.test(0);
            }

            ReturnCode get_return_code()
            {
                return static_cast<ReturnCode>(*(get_variable_header_start() + 1));
            }

            bool connection_was_accepted()
            {
                return get_return_code() == ACCEPTED;
            }

            void visit(IPacketReceiver& receiver) override;

        protected:
            int get_variable_header_length() const override
            {
                return 2;
            }
    };
}
