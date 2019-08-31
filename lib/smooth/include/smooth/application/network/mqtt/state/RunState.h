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

#include "ConnectedState.h"

namespace smooth::application::network::mqtt::state
{
    class RunState
        : public ConnectedState
    {
        public:
            RunState(MqttFSM<MQTTBaseState>& fsm, bool clean_session)
                    : ConnectedState(fsm, "RunState"),
                      reconnect_handled(false),
                      clean_session(clean_session)
            {
            }

            void tick() override;

            // For publishing
            void receive(packet::PubAck& pub_ack) override;

            void receive(packet::PubRec& pub_rec) override;

            void receive(packet::PubComp& pub_comp) override;

            // For subscribing
            void receive(packet::Publish& publish) override;

            void receive(packet::SubAck& sub_ack) override;

            void receive(packet::UnsubAck& unsub_ack) override;

            void receive(packet::PubRel& pub_rel) override;

        private:
            bool reconnect_handled;
            bool clean_session;
    };
}
