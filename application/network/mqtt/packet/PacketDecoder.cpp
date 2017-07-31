//
// Created by permal on 7/31/17.
//

#include <smooth/application/network/mqtt/packet/PacketDecoder.h>
#include <smooth/application/network/mqtt/packet/ConnAck.h>
#include <smooth/application/network/mqtt/packet/Publish.h>
#include <smooth/application/network/mqtt/packet/PubAck.h>
#include <smooth/application/network/mqtt/packet/PubRec.h>
#include <smooth/application/network/mqtt/packet/PubRel.h>
#include <smooth/application/network/mqtt/packet/PubComp.h>
#include <smooth/application/network/mqtt/packet/SubAck.h>
#include <smooth/application/network/mqtt/packet/UnsubAck.h>
#include <smooth/application/network/mqtt/packet/PingResp.h>

#include <string>
#include "esp_log.h"

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
                    // Decode messages from server to client
                    std::unique_ptr<MQTTPacket> PacketDecoder::decode_packet(const MQTTPacket& packet)
                    {
                        std::unique_ptr<MQTTPacket> res;

                        if (packet.get_mqtt_type() == CONNACK)
                        {
                            res = std::unique_ptr<MQTTPacket>(new ConnAck(packet));
                        }
                        else if (packet.get_mqtt_type() == PUBLISH)
                        {
                            res = std::unique_ptr<MQTTPacket>(new Publish(packet));
                        }
                        else if (packet.get_mqtt_type() == PUBACK)
                        {
                            res = std::unique_ptr<MQTTPacket>(new PubAck(packet));
                        }
                        else if (packet.get_mqtt_type() == PUBREC)
                        {
                            res = std::unique_ptr<MQTTPacket>(new PubRec(packet));
                        }
                        else if (packet.get_mqtt_type() == PUBREL)
                        {
                            res = std::unique_ptr<MQTTPacket>(new PubRel(packet));
                        }
                        else if (packet.get_mqtt_type() == PUBCOMP)
                        {
                            res = std::unique_ptr<MQTTPacket>(new PubComp(packet));
                        }
                        else if (packet.get_mqtt_type() == SUBACK)
                        {
                            res = std::unique_ptr<MQTTPacket>(new SubAck(packet));
                        }
                        else if (packet.get_mqtt_type() == UNSUBACK)
                        {
                            res = std::unique_ptr<MQTTPacket>(new UnsubAck(packet));
                        }
                        else if (packet.get_mqtt_type() == PINGRESP)
                        {
                            res = std::unique_ptr<MQTTPacket>(new PingResp(packet));
                        }

                        if( res )
                        {
                            res->dump();
                        }

                        return res;
                    }
                }
            }
        }
    }
}