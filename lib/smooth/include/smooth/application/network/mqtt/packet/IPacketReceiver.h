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

#pragma once

namespace smooth::application::network::mqtt::packet
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
            virtual ~IPacketReceiver() = default;

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
