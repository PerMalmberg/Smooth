// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <smooth/application/network/mqtt/state/RunState.h>
#include <smooth/application/network/mqtt/Publication.h>
#include <smooth/application/network/mqtt/Subscription.h>

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
