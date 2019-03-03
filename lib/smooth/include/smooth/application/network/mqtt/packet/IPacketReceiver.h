//
// Created by permal on 7/31/17.
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
                namespace packet
                {
                    class MQTTPacket;
                    class ConnAck;
                    class Publish;
                    class PubAck;
                    class PubRec;
                    class PubRel;
                    class PubComp;
                    class SubAck;
                    class Subscribe;
                    class Unsubscribe;
                    class UnsubAck;
                    class PingResp;

                    class IPacketReceiver
                    {
                        public:
                            virtual void receive(packet::MQTTPacket& raw_packet) = 0;
                            virtual void receive(packet::ConnAck& conn_ack) = 0;
                            virtual void receive(packet::Publish& publish) = 0;
                            virtual void receive(packet::PubAck& pub_ack) = 0;
                            virtual void receive(packet::PubRec& pub_rec) = 0;
                            virtual void receive(packet::PubRel& pub_rel) = 0;
                            virtual void receive(packet::PubComp& pub_comp) = 0;
                            virtual void receive(packet::Subscribe& sub) = 0;
                            virtual void receive(packet::SubAck& sub_ack) = 0;
                            virtual void receive(packet::Unsubscribe& sub_ack) = 0;
                            virtual void receive(packet::UnsubAck& unsub_ack) = 0;
                            virtual void receive(packet::PingResp& ping_resp) = 0;
                    };
                }
            }
        }
    }
}
