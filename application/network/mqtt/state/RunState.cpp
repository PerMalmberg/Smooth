//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/RunState.h>
#include <smooth/application/network/mqtt/ToBePublished.h>

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
                    void RunState::tick()
                    {
                        auto& to_publish = fsm.get_mqtt().get_to_be_published();

                        if (session_exists_on_server)
                        {
                            // Only do this once
                            session_exists_on_server = false;
                            to_publish.resend_outstanding_control_packet(fsm.get_mqtt());
                        }
                        else
                        {
                            to_publish.publish_next(fsm.get_mqtt());
                        }
                    }

                    void RunState::receive(packet::PubAck& pub_ack)
                    {
                        fsm.get_mqtt().get_to_be_published().receive(pub_ack, fsm.get_mqtt());
                    }

                    void RunState::receive(packet::PubRec& pub_rec)
                    {
                        fsm.get_mqtt().get_to_be_published().receive(pub_rec, fsm.get_mqtt());
                    }

                    void RunState::receive(packet::PubRel& pub_rel)
                    {

                    }

                    void RunState::receive(packet::PubComp& pub_comp)
                    {
                        fsm.get_mqtt().get_to_be_published().receive(pub_comp, fsm.get_mqtt());
                    }
                }
            }
        }
    }
}