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

#include <smooth/application/network/mqtt/state/DisconnectState.h>
#include <smooth/application/network/mqtt/state/IdleState.h>
#include <smooth/application/network/mqtt/packet/Disconnect.h>

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
