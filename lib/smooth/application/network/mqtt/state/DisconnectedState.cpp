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

#include <smooth/application/network/mqtt/state/DisconnectedState.h>
#include <smooth/application/network/mqtt/state/ConnectToBrokerState.h>
#include <smooth/application/network/mqtt/Subscription.h>
#include <smooth/application/network/mqtt/Publication.h>

namespace smooth::application::network::mqtt::state
{
    void DisconnectedState::enter_state()
    {
        fsm.get_mqtt().get_publication().handle_disconnect();
        fsm.get_mqtt().get_subscription().handle_disconnect();

        fsm.get_mqtt().set_keep_alive_timer(std::chrono::seconds(0));
        if (fsm.get_mqtt().is_auto_reconnect())
        {
            fsm.get_mqtt().start_reconnect();
        }
    }

    void DisconnectedState::event(const core::timer::TimerExpiredEvent& event)
    {
        if (event.get_id() == MQTT_FSM_RECONNECT_TIMER_ID)
        {
            fsm.get_mqtt().reconnect();
        }
    }

    void DisconnectedState::event(const core::network::event::ConnectionStatusEvent& event)
    {
        if (fsm.get_mqtt().is_auto_reconnect() && !event.is_connected())
        {
            fsm.get_mqtt().start_reconnect();
        }
        else if (event.is_connected())
        {
            fsm.set_state(new(fsm) ConnectToBrokerState(fsm));
        }
    }

}