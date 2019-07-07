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

#include <smooth/application/network/mqtt/state/MQTTBaseState.h>
#include <smooth/application/network/mqtt/state/IdleState.h>

using namespace smooth::core::fsm;

namespace smooth::application::network::mqtt::state
{
    MQTTBaseState::MQTTBaseState(MqttFSM<MQTTBaseState>& fsm, const char* name)
            : fsm(fsm), state_name()
    {
        std::fill(&state_name[0], &state_name[0] + sizeof(state_name), 0);
        strncpy(state_name, name, sizeof(state_name) - 1);
    }

    MQTTBaseState::~MQTTBaseState() = default;

    void MQTTBaseState::receive(packet::MQTTPacket&)
    {
    }

    void MQTTBaseState::receive(packet::ConnAck&)
    {
    }

    void MQTTBaseState::receive(packet::Publish&)
    {
    }

    void MQTTBaseState::receive(packet::PubAck&)
    {
    }

    void MQTTBaseState::receive(packet::PubRec&)
    {
    }

    void MQTTBaseState::receive(packet::PubRel&)
    {
    }

    void MQTTBaseState::receive(packet::PubComp&)
    {
    }

    void MQTTBaseState::receive(packet::SubAck&)
    {
    }

    void MQTTBaseState::receive(packet::Subscribe&)
    {
    }

    void MQTTBaseState::receive(packet::Unsubscribe&)
    {
    }

    void MQTTBaseState::receive(packet::UnsubAck&)
    {
    }

    void MQTTBaseState::receive(packet::PingResp&)
    {
    }
}