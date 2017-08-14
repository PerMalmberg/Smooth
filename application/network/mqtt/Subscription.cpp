//
// Created by permal on 8/14/17.
//

#include <smooth/application/network/mqtt/Subscription.h>
#include <algorithm>

using namespace std::chrono;

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                static const char* tag = "MQTT-Subscribe";

                void Subscription::subscribe(const std::string& topic, QoS qos)
                {
                    // Check active and not-yet completed subscriptions
                    auto it = active_subscription.find(topic);
                    if (it != active_subscription.end())
                    {
                        auto& sub = *it;
                        if (qos != sub.second)
                        {
                            packet::Subscribe s(topic, qos);
                            subscribing.push_back(InFlight<packet::Subscribe>(s));
                        }
                    }
                    else
                    {
                        // Simply add another subscription. If it already exists in the collection (possibly
                        // partly through the subscription process) we'll just handle it as any other subscription.
                        packet::Subscribe s(topic, qos);
                        subscribing.push_back(InFlight<packet::Subscribe>(s));
                    }
                }

                void Subscription::subscribe_next(IMqtt& mqtt)
                {
                    // We'll never have more than one outstanding subscription request.
                    // and it will always be the first one in the list.
                    if (subscribing.size() > 0)
                    {
                        auto& flight = subscribing.front();
                        if (flight.get_waiting_for() == PacketType::Reserved)
                        {
                            auto& packet = flight.get_packet();
                            if (mqtt.send_packet(packet))
                            {
                                flight.start_timer();
                                flight.set_wait_packet(PacketType::SUBACK);
                            }
                        }
                        else if (flight.get_elapsed_time() > seconds(15))
                        {
                            // Waited too long, force a reconnect.
                            ESP_LOGE("MQTT",
                                     "Too long since a reply was received to a subscription request, forcing reconnect.");
                            flight.stop_timer();
                            mqtt.reconnect();
                        }
                    }
                }

                void Subscription::handle_disconnect()
                {
                    // When disconnected, we need to move active subscriptions to the
                    // list of subscriptions net yet subscribed.
                    // Any subscription currently being subscribed should just be reset.

                    auto first = subscribing.begin();
                    if (first != subscribing.end())
                    {
                        auto& flight = *first;
                        flight.set_wait_packet(PacketType::Reserved);
                        flight.stop_timer();
                    }

                    // As we're actibely checking for duplicates when adding a subscription
                    // we must first remove it.
                    for( auto it = active_subscription.begin(); it != active_subscription.end();)
                    {
                        auto copy = *it;
                        it = active_subscription.erase(it);
                        subscribe(copy.first, copy.second);
                    }

                }

                void Subscription::receive(packet::SubAck& sub_ack, IMqtt& mqtt)
                {
                    auto first = subscribing.begin();
                    if (first != subscribing.end())
                    {
                        auto& flight = *first;
                        if (flight.get_waiting_for() == PacketType::SUBACK
                            && flight.get_packet().get_packet_identifier() == sub_ack.get_packet_identifier())
                        {
                            std::vector<std::pair<std::string, QoS>> topics{};
                            flight.get_packet().get_topics(topics);
                            for (auto& t : topics)
                            {
                                ESP_LOGV(tag, "Subscription of topic %s completed, QoS: %d", t.first.c_str(), t.second);
                                active_subscription.emplace(t.first, t.second);
                            }

                            subscribing.erase(subscribing.begin());
                        }
                    }
                }

                void Subscription::receive(packet::Publish& publish, IMqtt& mqtt)
                {
                    // Note: It is a valid use case where a Publish packet is received
                    // before a SubAck has been received for a subscription.

                    ESP_LOGV(tag, "To publish message!");

                    if (publish.get_qos() == QoS::AT_MOST_ONCE)
                    {
                        // Send to application
                    }
                    else if (publish.get_qos() == QoS::AT_LEAST_ONCE)
                    {

                    }
                    else if (publish.get_qos() == QoS::EXACTLY_ONCE)
                    {

                    }
                }

                void Subscription::receive(packet::UnsubAck& unsub_ack, IMqtt& mqtt)
                {
                }

                void Subscription::receive(packet::PubRel& pub_rel, IMqtt& mqtt)
                {
                }
            }
        }
    }
}

