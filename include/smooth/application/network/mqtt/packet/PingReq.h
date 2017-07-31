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
                    class PingReq
                            : public MQTTPacket
                    {
                        public:
                            PingReq();

                    };
                }
            }
        }
    }
}
