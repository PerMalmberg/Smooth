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

#include <memory>
#include <string>
#include "smooth/core/logging/log.h"
#include "smooth/application/network/mqtt/packet/PacketDecoder.h"
#include "smooth/application/network/mqtt/packet/ConnAck.h"
#include "smooth/application/network/mqtt/packet/Publish.h"
#include "smooth/application/network/mqtt/packet/PubAck.h"
#include "smooth/application/network/mqtt/packet/PubRec.h"
#include "smooth/application/network/mqtt/packet/PubRel.h"
#include "smooth/application/network/mqtt/packet/PubComp.h"
#include "smooth/application/network/mqtt/packet/SubAck.h"
#include "smooth/application/network/mqtt/packet/UnsubAck.h"
#include "smooth/application/network/mqtt/packet/PingResp.h"
#include "smooth/application/network/mqtt/Logging.h"

using namespace smooth::core::logging;
using namespace std;

namespace smooth::application::network::mqtt::packet
{
    // Decode messages from server to client
    std::unique_ptr<MQTTPacket> PacketDecoder::decode_packet(const MQTTPacket& packet)
    {
        std::unique_ptr<MQTTPacket> res;
        using namespace core::util;

        if (packet.is_too_big())
        {
            // We can't do anything with packets that are too big as their contents
            // is invalid; not even the variable header can be considered intact.
            Log::verbose(mqtt_log_tag, "Too big packet discarded");
        }
        else
        {
            if (packet.get_mqtt_type() == CONNACK)
            {
                res = make_unique<ConnAck>(packet);
            }
            else if (packet.get_mqtt_type() == PUBLISH)
            {
                res = make_unique<Publish>(packet);
            }
            else if (packet.get_mqtt_type() == PUBACK)
            {
                res = make_unique<PubAck>(packet);
            }
            else if (packet.get_mqtt_type() == PUBREC)
            {
                res = make_unique<PubRec>(packet);
            }
            else if (packet.get_mqtt_type() == PUBREL)
            {
                res = make_unique<PubRel>(packet);
            }
            else if (packet.get_mqtt_type() == PUBCOMP)
            {
                res = make_unique<PubComp>(packet);
            }
            else if (packet.get_mqtt_type() == SUBACK)
            {
                res = make_unique<SubAck>(packet);
            }
            else if (packet.get_mqtt_type() == UNSUBACK)
            {
                res = make_unique<UnsubAck>(packet);
            }
            else if (packet.get_mqtt_type() == PINGRESP)
            {
                res = make_unique<PingResp>(packet);
            }
        }

        if (res)
        {
            if (res->validate_packet())
            {
                res->dump("Incoming");
            }
            else
            {
                res.reset();
            }
        }

        return res;
    }
}
