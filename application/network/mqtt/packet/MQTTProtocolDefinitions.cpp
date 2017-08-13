//
// Created by permal on 8/13/17.
//

#include <smooth/application/network/mqtt/MQTTProtocolDefinitions.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                const std::unordered_map<PacketType, const char*, PacketTypeHash> packet_type_as_string
                        {
                                {Reserved,    "Reserved"},
                                {CONNECT,     "Connect"},
                                {CONNACK,     "ConAck"},
                                {PUBLISH,     "Publish"},
                                {PUBACK,      "PubAck"},
                                {PUBREC,      "PubRec"},
                                {PUBREL,      "PubRel"},
                                {PUBCOMP,     "PubComp"},
                                {SUBSCRIBE,   "Subscribe"},
                                {SUBACK,      "SubAck"},
                                {UNSUBSCRIBE, "Unsubscribe"},
                                {UNSUBACK,    "UnsubAck"},
                                {PINGREQ,     "PingReq"},
                                {PINGRESP,    "PingResp"},
                                {DISCONNECT,  "Disconnect"},
                                {Reserved_2,  "Reserved_2"}
                        };

            }
        }
    }
}