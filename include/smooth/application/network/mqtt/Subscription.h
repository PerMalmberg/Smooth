//
// Created by permal on 8/14/17.
//

#pragma once

#include <vector>
#include <unordered_map>
#include <smooth/application/network/mqtt/packet/PubAck.h>
#include <smooth/application/network/mqtt/packet/PubComp.h>
#include <smooth/application/network/mqtt/packet/Publish.h>
#include <smooth/application/network/mqtt/packet/PubRec.h>
#include <smooth/application/network/mqtt/packet/PubRel.h>
#include <smooth/application/network/mqtt/packet/SubAck.h>
#include <smooth/application/network/mqtt/packet/Subscribe.h>
#include <smooth/application/network/mqtt/packet/UnsubAck.h>
#include <smooth/application/network/mqtt/packet/Unsubscribe.h>
#include <smooth/application/network/mqtt/IMqtt.h>
#include <smooth/application/network/mqtt/InFlight.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                class Subscription
                {
                    public:
                        void subscribe(const std::string& topic, QoS qos);
                        void unsubscribe(const std::string& topic);

                        void receive(packet::Publish& publish, IMqtt& mqtt);
                        void receive(packet::SubAck& sub_ack, IMqtt& mqtt);
                        void receive(packet::UnsubAck& unsub_ack, IMqtt& mqtt);
                        void receive(packet::PubRel& pub_rel, IMqtt& mqtt);

                        void subscribe_next(IMqtt& mqtt);
                        void handle_disconnect();
                    private:
                        void forward_to_application(const packet::Publish& publish, IMqtt& mqtt);

                        template<typename T>
                        bool send_control_packet(std::vector<InFlight<T>>& in_flight, PacketType wait_for, IMqtt& mqtt, const char* control_type)
                        {
                            bool all_ok = true;

                            if (in_flight.size() > 0)
                            {
                                auto& flight = in_flight.front();
                                if (flight.get_waiting_for() == PacketType::Reserved)
                                {
                                    auto& packet = flight.get_packet();
                                    if (mqtt.send_packet(packet))
                                    {
                                        flight.start_timer();
                                        flight.set_wait_packet(wait_for);
                                    }
                                }
                                else if (flight.get_elapsed_time() > std::chrono::seconds(15))
                                {
                                    // Waited too long, force a reconnect.
                                    ESP_LOGE("MQTT",
                                             "Too long since a reply was received to a %s request, forcing reconnect.", control_type);
                                    flight.stop_timer();
                                    mqtt.reconnect();
                                    all_ok = false;
                                }
                            }

                            return all_ok;
                        }

                        template<typename T>
                        void reset_control_packet(std::vector<InFlight<T>>& in_flight)
                        {
                            auto first = in_flight.begin();
                            if (first != in_flight.end())
                            {
                                auto& flight = *first;
                                flight.set_wait_packet(PacketType::Reserved);
                                flight.stop_timer();
                            }
                        }

                        std::unordered_map<uint16_t, InFlight<packet::Publish>> receiving{};
                        std::vector<InFlight<packet::Subscribe>> subscribing{};
                        std::unordered_map<std::string, QoS> active_subscription{};
                        std::vector<InFlight<packet::Unsubscribe>> unsubscribing{};
                };
            }
        }
    }
}

