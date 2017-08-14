//
// Created by permal on 8/14/17.
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
                template<typename T>
                class InFlight
                {
                    public:
                        InFlight(T& p)
                                : p(p)
                        {
                        }

                        T& get_packet()
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
                        T p{};
                        PacketType waiting_for_packet = PacketType::Reserved;
                        core::timer::PerfCount timer{};
                };
            }
        }
    }
}
