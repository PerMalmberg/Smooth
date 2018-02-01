//
// Created by permal on 7/22/17.
//

#pragma once

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
                    class PingResp
                            : public MQTTPacket
                    {
                        public:
                            PingResp() = default;

                            explicit PingResp(const MQTTPacket& packet) : MQTTPacket(packet)
                            {
                            }

                            void visit( IPacketReceiver& receiver ) override;
                    };
                }
            }
        }
    }
}
