//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/MQTTBaseState.h>
#include <smooth/application/network/mqtt/state/IdleState.h>

using namespace smooth::core::fsm;

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                namespace state
                {
                    MQTTBaseState::MQTTBaseState(MqttFSM<MQTTBaseState>& fsm, const char* name)
                            : fsm(fsm)
                    {
                        std::fill(&state_name[0], &state_name[0] + sizeof(state_name), 0);
                        strncpy(state_name, name, sizeof(state_name) - 1);
                    }

                    MQTTBaseState::~MQTTBaseState() = default;

                    void MQTTBaseState::receive(packet::MQTTProtocol&)
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
            }
        }
    }
}