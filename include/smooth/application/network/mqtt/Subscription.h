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
#include <smooth/application/network/mqtt/IMqttClient.h>
#include <smooth/application/network/mqtt/InFlight.h>
#include <smooth/application/network/mqtt/Logging.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

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

                        void receive(packet::Publish& publish, IMqttClient& mqtt);
                        void receive(packet::SubAck& sub_ack, IMqttClient& mqtt);
                        void receive(packet::UnsubAck& unsub_ack, IMqttClient& mqtt);
                        void receive(packet::PubRel& pub_rel, IMqttClient& mqtt);

                        void subscribe_next(IMqttClient& mqtt);
                        void handle_disconnect();
                    private:
                        void forward_to_application(const packet::Publish& publish, IMqttClient& mqtt);

                        template<typename T>
                        bool send_control_packet(std::vector<InFlight<T>>& in_flight, PacketType wait_for, IMqttClient& mqtt, const char* control_type)
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
                                    // Waited too long, force a disconnect.
                                    Log::error(mqtt_log_tag, Format(
                                             "Too long since a reply was received to a {1} request, forcing disconnect.", Str(control_type)));
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

