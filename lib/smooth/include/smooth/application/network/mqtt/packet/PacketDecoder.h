//
// Created by permal on 7/31/17.
//

#pragma once

#include <memory>
#include <smooth/application/network/mqtt/packet/MQTTProtocol.h>

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
                    class PacketDecoder
                    {
                        public:
                            std::unique_ptr<MQTTProtocol> decode_packet(const MQTTProtocol& packet);
                    };
                }
            }
        }
    }
}