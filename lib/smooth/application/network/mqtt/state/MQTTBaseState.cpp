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

#include "smooth/application/network/mqtt/state/MQTTBaseState.h"
#include "smooth/application/network/mqtt/state/IdleState.h"

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
