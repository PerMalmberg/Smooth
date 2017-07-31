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

                    MQTTBaseState::~MQTTBaseState()
                    {

                    }

                    void MQTTBaseState::receive(packet::MQTTPacket& raw_packet) 
                    {

                    }

                    void MQTTBaseState::receive(packet::ConnAck& conn_ack) 
                    {

                    }

                    void MQTTBaseState::receive(packet::Publish& publish) 
                    {

                    }

                    void MQTTBaseState::receive(packet::PubAck& pub_ack) 
                    {
                    }

                    void MQTTBaseState::receive(packet::PubRec& pub_rec) 
                    {
                    }

                    void MQTTBaseState::receive(packet::PubRel& pub_rel) 
                    {
                    }

                    void MQTTBaseState::receive(packet::PubComp& pub_comp) 
                    {
                    }

                    void MQTTBaseState::receive(packet::SubAck& sub_ack) 
                    {
                    }

                    void MQTTBaseState::receive(packet::UnsubAck& unsub_ack) 
                    {
                    }

                    void MQTTBaseState::receive(packet::PingResp& ping_resp) 
                    {
                    }
                }
            }
        }
    }
}