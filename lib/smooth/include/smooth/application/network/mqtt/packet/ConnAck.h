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

#include "smooth/application/network/mqtt/packet/MQTTProtocol.h"

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
