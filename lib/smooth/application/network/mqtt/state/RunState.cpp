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

#include "smooth/application/network/mqtt/state/RunState.h"
#include "smooth/application/network/mqtt/Publication.h"
#include "smooth/application/network/mqtt/Subscription.h"

namespace smooth::application::network::mqtt::state
{
    void RunState::tick()
    {
        auto& publication = fsm.get_mqtt().get_publication();

        if (reconnect_handled)
        {
            publication.publish_next(fsm.get_mqtt());
            auto& subscription = fsm.get_mqtt().get_subscription();
            subscription.subscribe_next(fsm.get_mqtt());
        }
        else
        {
            // Only do this once
            reconnect_handled = true;
            publication.resend_outstanding_control_packet(fsm.get_mqtt(), clean_session);
        }
    }

    void RunState::receive(packet::PubAck& pub_ack)
    {
        fsm.get_mqtt().get_publication().receive(pub_ack, fsm.get_mqtt());
    }

    void RunState::receive(packet::PubRec& pub_rec)
    {
        fsm.get_mqtt().get_publication().receive(pub_rec, fsm.get_mqtt());
    }

    void RunState::receive(packet::PubRel& pub_rel)
    {
        fsm.get_mqtt().get_subscription().receive(pub_rel, fsm.get_mqtt());
    }

    void RunState::receive(packet::PubComp& pub_comp)
    {
        fsm.get_mqtt().get_publication().receive(pub_comp, fsm.get_mqtt());
    }

    void RunState::receive(packet::Publish& publish)
    {
        fsm.get_mqtt().get_subscription().receive(publish, fsm.get_mqtt());
    }

    void RunState::receive(packet::SubAck& sub_ack)
    {
        fsm.get_mqtt().get_subscription().receive(sub_ack, fsm.get_mqtt());
    }

    void RunState::receive(packet::UnsubAck& unsub_ack)
    {
        fsm.get_mqtt().get_subscription().receive(unsub_ack, fsm.get_mqtt());
    }
}
