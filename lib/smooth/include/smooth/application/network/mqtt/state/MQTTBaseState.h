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

#pragma once

#include <smooth/core/network/event/DataAvailableEvent.h>
#include <smooth/core/network/event/ConnectionStatusEvent.h>
#include <smooth/core/network/event/TransmitBufferEmptyEvent.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/timer/Timer.h>
#include <smooth/core/timer/TimerExpiredEvent.h>
#include <smooth/application/network/mqtt/packet/IPacketReceiver.h>
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

            const char * get_name() const
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
