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

#include "smooth/application/network/mqtt/state/DisconnectedState.h"
#include "smooth/application/network/mqtt/state/ConnectToBrokerState.h"
#include "smooth/application/network/mqtt/Subscription.h"
#include "smooth/application/network/mqtt/Publication.h"

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
