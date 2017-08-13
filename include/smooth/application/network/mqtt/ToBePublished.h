//
// Created by permal on 8/12/17.
//

#pragma once

#include <vector>
#include <chrono>
#include <smooth/core/timer/PerfCount.h>
#include <smooth/application/network/mqtt/packet/PubAck.h>
#include <smooth/application/network/mqtt/packet/PubComp.h>
#include <smooth/application/network/mqtt/packet/Publish.h>
#include <smooth/application/network/mqtt/packet/PubRec.h>
#include <smooth/application/network/mqtt/IMqtt.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                class ToBePublished
                {
                    public:
                        void
                        publish(const std::string& topic, const uint8_t* data, int length, mqtt::QoS qos, bool retain);

                        void publish_next(IMqtt& mqtt);

                        void resend_outstanding_control_packet(IMqtt& mqtt);

                        void receive(packet::PubAck& pub_ack, IMqtt& mqtt);

                        void receive(packet::PubRec& pub_rec, IMqtt& mqtt);

                        void receive(packet::PubComp& pub_rel, IMqtt& mqtt);

                    private:

                        class InFlight
                        {
                            public:
                                InFlight(packet::Publish& p)
                                        : p(p)
                                {
                                }

                                packet::Publish& get_packet()
                                {
                                    return p;
                                }

                                PacketType get_waiting_for()
                                {
                                    return waiting_for_packet;
                                }

                                void set_wait_packet(PacketType type)
                                {
                                    waiting_for_packet = type;
                                }

                                void start_timer()
                                {
                                    timer.start();
                                }

                                void stop_timer()
                                {
                                    timer.stop();
                                }

                                std::chrono::milliseconds get_elapsed_time()
                                {
                                    return std::chrono::duration_cast<std::chrono::seconds>(timer.get_running_time());
                                }

                            private:
                                packet::Publish p{};
                                PacketType waiting_for_packet = PacketType::Reserved;
                                core::timer::PerfCount timer{};
                        };

                        std::vector<InFlight> in_progress{};
                };
            }
        }
    }
}