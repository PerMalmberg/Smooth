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

#include "smooth/core/network/event/DataAvailableEvent.h"
#include "smooth/core/network/event/ConnectionStatusEvent.h"
#include "smooth/core/network/event/TransmitBufferEmptyEvent.h"
#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/timer/Timer.h"
#include "smooth/core/timer/TimerExpiredEvent.h"
#include "smooth/application/network/mqtt/packet/IPacketReceiver.h"
#include "MqttFSM.h"

namespace smooth::application::network::mqtt::state
{
    class MQTTBaseState
        :
        public core::ipc::IEventListener<core::network::event::TransmitBufferEmptyEvent>,
        public core::ipc::IEventListener<core::network::event::ConnectionStatusEvent>,
        public core::ipc::IEventListener<core::timer::TimerExpiredEvent>,
        public mqtt::packet::IPacketReceiver
    {
        public:
            MQTTBaseState(MqttFSM<MQTTBaseState>& fsm, const char* name);

            ~MQTTBaseState() override;

            virtual void enter_state()
            {
            }

            virtual void leave_state()
            {
            }

            [[nodiscard]] const char* get_name() const
            {
                return state_name;
            }

            virtual void tick()
            {
            }

            void event(const core::network::event::TransmitBufferEmptyEvent&) override
            {
            }

            void event(const core::network::event::ConnectionStatusEvent&) override
            {
            }

            void event(const core::timer::TimerExpiredEvent&) override
            {
            }

            void receive(packet::MQTTPacket& raw_packet) override;

            void receive(packet::ConnAck& conn_ack) override;

            void receive(packet::Publish& publish) override;

            void receive(packet::PubAck& pub_ack) override;

            void receive(packet::PubRec& pub_rec) override;

            void receive(packet::PubRel& pub_rel) override;

            void receive(packet::PubComp& pub_comp) override;

            void receive(packet::SubAck& sub_ack) override;

            void receive(packet::Subscribe& sub) override;

            void receive(packet::Unsubscribe& unsub) override;

            void receive(packet::UnsubAck& unsub_ack) override;

            void receive(packet::PingResp& ping_resp) override;

        protected:
            MqttFSM<MQTTBaseState>& fsm;
            char state_name[20];
    };
}
