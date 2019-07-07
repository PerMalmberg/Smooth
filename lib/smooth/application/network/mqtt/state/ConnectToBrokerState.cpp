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

#include <smooth/application/network/mqtt/state/ConnectToBrokerState.h>
#include <smooth/application/network/mqtt/state/RunState.h>
#include <smooth/application/network/mqtt/state/IdleState.h>
#include <smooth/application/network/mqtt/packet/Connect.h>
#include <smooth/application/network/mqtt/packet/ConnAck.h>

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