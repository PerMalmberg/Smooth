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
    class Unsubscribe
        : public MQTTPacket
    {
        public:
            Unsubscribe() = default;

            explicit Unsubscribe(const std::string& topic)
            {
                set_header(UNSUBSCRIBE, 0x2);
                std::vector<uint8_t> data;
                append_msb_lsb(PacketIdentifierFactory::get_id(), data);
                append_string(topic, data);
                apply_constructed_data(data);
            }

            explicit Unsubscribe(const MQTTPacket& packet)
                    : MQTTPacket(packet)
            {
            }

            uint16_t get_packet_identifier() const override
            {
                return read_packet_identifier(get_variable_header_start());
            }

            void get_topics(std::vector<std::string>& topics) const;

            void visit(IPacketReceiver& receiver) override;

        protected:
            int get_variable_header_length() const override
            {
                return 2;
            }

            bool has_packet_identifier() const override
            {
                return true;
            }

            bool has_payload() const override
            {
                return true;
            }
    };
}
