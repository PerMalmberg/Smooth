//
// Created by permal on 7/30/17.
//

#pragma once

#include <array>
#include <smooth/core/network/DataAvailableEvent.h>
#include <smooth/core/network/ConnectionStatusEvent.h>
#include <smooth/core/network/TransmitBufferEmptyEvent.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/timer/Timer.h>
#include <smooth/core/timer/TimerExpiredEvent.h>
#include <smooth/application/network/mqtt/packet/IPacketReceiver.h>
#include "MqttFSM.h"

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
                    class MQTTBaseState
                            :
                                    public core::ipc::IEventListener<core::network::TransmitBufferEmptyEvent>,
                                    public core::ipc::IEventListener<core::network::ConnectionStatusEvent>,
                                    public core::ipc::IEventListener<core::timer::TimerExpiredEvent>,
                                    public mqtt::packet::IPacketReceiver
                    {
                        public:
                            MQTTBaseState(MqttFSM <MQTTBaseState>& fsm, const char* name);

                            virtual ~MQTTBaseState();

                            virtual void enter_state()
                            {
                            }

                            virtual void leave_state()
                            {
                            }

                            const char* get_name() const
                            {
                                return state_name;
                            }

                            virtual void tick()
                            {
                            }

                            void event(const core::network::TransmitBufferEmptyEvent& event) override
                            {
                            }

                            void event(const core::network::ConnectionStatusEvent& event) override
                            {
                            }

                            void event(const core::timer::TimerExpiredEvent& event) override
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
                            MqttFSM <MQTTBaseState>& fsm;
                            char state_name[20];
                    };
                }
            }
        }
    }
}
