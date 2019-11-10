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

#include "smooth/application/network/mqtt/state/ConnectToBrokerState.h"
#include "smooth/application/network/mqtt/state/RunState.h"
#include "smooth/application/network/mqtt/state/IdleState.h"
#include "smooth/application/network/mqtt/packet/Connect.h"
#include "smooth/application/network/mqtt/packet/ConnAck.h"

namespace smooth::application::network::mqtt::state
{
    void ConnectToBrokerState::enter_state()
    {
        packet::Connect con(fsm.get_mqtt().get_client_id(), fsm.get_mqtt().get_keep_alive());

        is_using_clean_session = con.get_clean_session();
        fsm.get_mqtt().send_packet(con);
        fsm.get_mqtt().set_keep_alive_timer(fsm.get_mqtt().get_keep_alive());
    }

    void ConnectToBrokerState::receive(packet::ConnAck& conn_ack)
    {
        if (conn_ack.connection_was_accepted())
        {
            fsm.set_state(new(fsm) RunState(fsm, is_using_clean_session));
        }
        else
        {
            fsm.set_state(new(fsm) IdleState(fsm));
        }
    }
}
