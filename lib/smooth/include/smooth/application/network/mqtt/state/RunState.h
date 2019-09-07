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
