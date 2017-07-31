//
// Created by permal on 7/22/17.
//

#pragma once

#include <chrono>
#include <smooth/application/network/mqtt/packet/MQTTPacket.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                namespace packet
                {
                    class Connect
                            : public MQTTPacket
                    {
                        public:
                            Connect() = default;
                            Connect(const std::string& client_id, std::chrono::seconds keep_alive);
                        private:
                            std::vector<uint8_t> variable_header{};
                    };
                }
            }
        }
    }
}
