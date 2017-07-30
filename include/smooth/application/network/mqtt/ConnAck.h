//
// Created by permal on 7/22/17.
//

#pragma once

#include <smooth/application/network/mqtt/MQTTPacket.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                class ConnAck
                        : public MQTTPacket
                {
                    public:
                        ConnAck() = default;

                        Connect(const std::vector<uint8_t> data) : MQTTPacket(data)
                        {
                        }
                };
            }
        }
    }
}
