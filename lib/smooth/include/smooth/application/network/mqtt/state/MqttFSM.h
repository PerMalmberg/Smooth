//
// Created by permal on 7/30/17.
//

#pragma once

#include <smooth/core/fsm/StaticFSM.h>
#include <smooth/core/timer/TimerExpiredEvent.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/event/DataAvailableEvent.h>
#include <smooth/core/network/event/ConnectionStatusEvent.h>
#include <smooth/core/network/event/TransmitBufferEmptyEvent.h>
#include <smooth/application/network/mqtt/packet/MQTTProtocol.h>
#include <smooth/application/network/mqtt/IMqttClient.h>
#include <smooth/application/network/mqtt/packet/PacketDecoder.h>
#include <smooth/application/network/mqtt/Logging.h>
#include "MqttFsmConstants.h"

using namespace smooth::core::logging;

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
                    template<typename BaseState>
                    class MqttFSM
                            : public core::fsm::StaticFSM<BaseState, MQTT_FSM_STATE_SIZE>,
                              public core::ipc::IEventListener<core::network::event::TransmitBufferEmptyEvent>,
                              public core::ipc::IEventListener<core::network::event::ConnectionStatusEvent>,
                              public core::ipc::IEventListener<core::timer::TimerExpiredEvent>
                    {
                        public:
                            explicit MqttFSM(mqtt::IMqttClient& mqtt) : mqtt(mqtt)
                            {
                            }

                            void entering_state(BaseState* state) override;

                            void leaving_state(BaseState* state) override;

                            void tick();
                            void event(const core::network::event::TransmitBufferEmptyEvent& event) override;
                            void event(const core::network::event::ConnectionStatusEvent& event) override;
                            void event(const core::timer::TimerExpiredEvent& event) override;

                            void packet_received(const packet::MQTTPacket& packet);

                            mqtt::IMqttClient& get_mqtt() const
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
                        Log::debug(mqtt_log_tag, Format("Entering {1}", Str(state->get_name())));
                    }

                    template<typename BaseState>
                    void MqttFSM<BaseState>::leaving_state(BaseState* state)
                    {
                        Log::debug(mqtt_log_tag, Format("Leaving {1}", Str(state->get_name())));
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
                            if( p )
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
            }
        }
    }
}
