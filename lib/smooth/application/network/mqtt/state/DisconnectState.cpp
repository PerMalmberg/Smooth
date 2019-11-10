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

#include "smooth/application/network/mqtt/state/DisconnectState.h"
#include "smooth/application/network/mqtt/state/IdleState.h"
#include "smooth/application/network/mqtt/packet/Disconnect.h"

namespace smooth::application::network::mqtt::state
{
    void DisconnectState::enter_state()
    {
        packet::Disconnect disconnect;
        fsm.get_mqtt().send_packet(disconnect);
        elapsed_time.start();
    }

    void DisconnectState::event(const core::network::event::TransmitBufferEmptyEvent&)
    {
        // Disconnect sent, we're done
        fsm.set_state(new(fsm) IdleState(fsm));
    }

    void DisconnectState::tick()
    {
        if (elapsed_time.get_running_time() > std::chrono::seconds(2))
        {
            // Timeout, move on
            fsm.set_state(new(fsm) IdleState(fsm));
        }
    }
}
