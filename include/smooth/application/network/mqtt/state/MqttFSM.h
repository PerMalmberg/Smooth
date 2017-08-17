//
// Created by permal on 7/30/17.
//

#pragma once

#include <smooth/core/fsm/StaticFSM.h>
#include <smooth/core/timer/TimerExpiredEvent.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/network/DataAvailableEvent.h>
#include <smooth/core/network/ConnectionStatusEvent.h>
#include <smooth/core/network/TransmitBufferEmptyEvent.h>
#include <smooth/application/network/mqtt/packet/MQTTPacket.h>
#include <smooth/application/network/mqtt/IMqttClient.h>
#include <smooth/application/network/mqtt/packet/PacketDecoder.h>
#include <smooth/application/network/mqtt/Logging.h>
#include "MqttFsmConstants.h"

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
                              public core::ipc::IEventListener<core::network::TransmitBufferEmptyEvent>,
                              public core::ipc::IEventListener<core::network::ConnectionStatusEvent>,
                              public core::ipc::IEventListener<core::timer::TimerExpiredEvent>
                    {
                        public:
                            explicit MqttFSM(mqtt::IMqttClient& mqtt) : mqtt(mqtt)
                            {
                            }

                            virtual void entering_state(BaseState* state) override;

                            virtual void leaving_state(BaseState* state) override;

                            void tick();
                            void event(const core::network::TransmitBufferEmptyEvent& event) override;
                            void event(const core::network::ConnectionStatusEvent& event) override;
                            void event(const core::timer::TimerExpiredEvent& event) override;

                            void packet_received(const packet::MQTTPacket& event);

                            void disconnect_event();

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
                        ESP_LOGD(mqtt_log_tag, "Entering %s", state->get_name());
                    }

                    template<typename BaseState>
                    void MqttFSM<BaseState>::leaving_state(BaseState* state)
                    {
                        ESP_LOGD(mqtt_log_tag, "Leaving %s", state->get_name());
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
                    void MqttFSM<BaseState>::event(const core::network::TransmitBufferEmptyEvent& event)
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
                    void MqttFSM<BaseState>::event(const core::network::ConnectionStatusEvent& event)
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

                    template<typename BaseState>
                    void MqttFSM<BaseState>::disconnect_event()
                    {
                        if (this->get_state() != nullptr)
                        {
                            this->get_state()->disconnect_event();
                        }
                    }
                }
            }
        }
    }
}
