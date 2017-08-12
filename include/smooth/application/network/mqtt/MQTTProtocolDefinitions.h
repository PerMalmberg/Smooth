//
// Created by permal on 7/22/17.
//

#pragma once

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                enum PacketType
                {
                    Reserved = 0,
                    CONNECT = 1,
                    CONNACK = 2,
                    PUBLISH = 3,
                    PUBACK = 4,
                    PUBREC = 5,
                    PUBREL = 6,
                    PUBCOMP = 7,
                    SUBSCRIBE = 8,
                    SUBACK = 9,
                    UNSUBSCRIBE = 10,
                    UNSUBACK = 11,
                    PINGREQ = 12,
                    PINGRESP = 13,
                    DISCONNECT = 14,
                    Reserved_2 = 15
                };

                enum QoS
                {
                    AT_MOST_ONCE = 0,
                    AT_LEAST_ONCE = 1,
                    EXACTLY_ONCE = 2
                };
            }
        }
    }
}