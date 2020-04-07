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

#pragma once

#include "smooth/core/fsm/StaticFSM.h"
#include "smooth/core/timer/TimerExpiredEvent.h"
#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/network/event/DataAvailableEvent.h"
#include "smooth/core/network/event/ConnectionStatusEvent.h"
#include "smooth/core/network/event/TransmitBufferEmptyEvent.h"
#include "smooth/application/network/mqtt/packet/MQTTProtocol.h"
#include "smooth/application/network/mqtt/IMqttClient.h"
#include "smooth/application/network/mqtt/packet/PacketDecoder.h"
#include "smooth/application/network/mqtt/Logging.h"
#include "MqttFsmConstants.h"

using namespace smooth::core::logging;

namespace smooth::application::network::mqtt::state
{
    template<typename BaseState>
    class MqttFSM
        : public core::fsm::StaticFSM<BaseState, MQTT_FSM_STATE_SIZE>,
        public core::ipc::IEventListener<core::network::event::TransmitBufferEmptyEvent>,
        public core::ipc::IEventListener<core::network::event::ConnectionStatusEvent>,
        public core::ipc::IEventListener<core::timer::TimerExpiredEvent>
    {
        public:
            explicit MqttFSM(mqtt::IMqttClient& mqtt)
                    : mqtt(mqtt)
            {
            }

            void entering_state(BaseState* state) override;

            void leaving_state(BaseState* state) override;

            void tick();

            void event(const core::network::event::TransmitBufferEmptyEvent& event) override;

            void event(const core::network::event::ConnectionStatusEvent& event) override;

            void event(const core::timer::TimerExpiredEvent& event) override;

            void packet_received(const packet::MQTTPacket& packet);

            [[nodiscard]] mqtt::IMqttClient& get_mqtt() const
            {
                return mqtt;
            }

        private:
            mqtt::IMqttClient& mqtt;
            packet::PacketDecoder decoder;
    };

    template<typename BaseState>
    void MqttFSM<BaseState>::entering_state(BaseState* state)
    {
        Log::debug(mqtt_log_tag, "Entering {}", state->get_name());
    }

    template<typename BaseState>
    void MqttFSM<BaseState>::leaving_state(BaseState* state)
    {
        Log::debug(mqtt_log_tag, "Leaving {}", state->get_name());
    }

    template<typename BaseState>
    void MqttFSM<BaseState>::tick()
    {
        if (this->get_state() != nullptr)
        {
            this->get_state()->tick();
        }
    }

    template<typename BaseState>
    void MqttFSM<BaseState>::event(const core::network::event::TransmitBufferEmptyEvent& event)
    {
        if (this->get_state() != nullptr)
        {
            this->get_state()->event(event);
        }
    }

    template<typename BaseState>
    void MqttFSM<BaseState>::packet_received(const packet::MQTTPacket& packet)
    {
        if (this->get_state() != nullptr)
        {
            // Decode the message and forward it to the state
            auto p = decoder.decode_packet(packet);

            if (p)
            {
                p->visit(*this->get_state());
            }
        }
    }

    template<typename BaseState>
    void MqttFSM<BaseState>::event(const core::network::event::ConnectionStatusEvent& event)
    {
        if (this->get_state() != nullptr)
        {
            this->get_state()->event(event);
        }
    }

    template<typename BaseState>
    void MqttFSM<BaseState>::event(const core::timer::TimerExpiredEvent& event)
    {
        if (this->get_state() != nullptr)
        {
            this->get_state()->event(event);
        }
    }
}
