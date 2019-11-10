/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <algorithm>
#include "smooth/application/network/mqtt/Subscription.h"
#include "smooth/application/network/mqtt/packet/Subscribe.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;
using namespace std::chrono;

namespace smooth::application::network::mqtt
{
    void Subscription::subscribe(const std::string& topic, QoS qos)
    {
        std::lock_guard<std::mutex> lock(guard);

        // Check active and not-yet completed subscriptions
        internal_subscribe(topic, qos);
    }

    void Subscription::internal_subscribe(const std::string& topic, const QoS& qos)
    {
        // Intentionally not locking here - check call-sites.

        auto it = active_subscription.find(topic);

        if (it == active_subscription.end())
        {
            // Simply add another subscription. If it already exists in the collection (possibly
            // partly through the subscription process) we'll just handle it as any other subscription.
            packet::Subscribe s(topic, qos);
            subscribing.emplace_back(s);
        }
        else
        {
            auto& sub = *it;

            if (qos != sub.second)
            {
                packet::Subscribe s(topic, qos);
                subscribing.emplace_back(s);
            }
        }
    }

    void Subscription::unsubscribe(const std::string& topic)
    {
        std::lock_guard<std::mutex> lock(guard);

        // Just enqueue for transfer to server.
        packet::Unsubscribe us(topic);
        unsubscribing.emplace_back(us);
    }

    void Subscription::subscribe_next(IMqttClient& mqtt)
    {
        std::lock_guard<std::mutex> lock(guard);

        // We'll never have more than one outstanding subscription request.
        // and it will always be the first one in the list.
        bool all_ok = send_control_packet(subscribing, PacketType::SUBACK, mqtt, "subscription");
        all_ok = all_ok && send_control_packet(unsubscribing, PacketType::UNSUBACK, mqtt, "unsubscription");

        if (all_ok)
        {
            // Resend any unacknowledged Publish messages
            for (auto& pair : receiving)
            {
                auto& flight = pair.second;

                if (flight.get_waiting_for() == PacketType::PUBREL)
                {
                    if (flight.get_elapsed_time() > std::chrono::seconds{ 5 })
                    {
                        // Send a PubRec message.
                        auto& packet = flight.get_packet();
                        packet::PubRec rec(packet.get_packet_identifier());

                        // If we can enqueue it, restart the timer to give the response a chance to arrive.
                        // Otherwise, another try will happen the next turn.
                        if (mqtt.send_packet(rec))
                        {
                            flight.start_timer();
                        }
                    }
                }
            }
        }
    }

    void Subscription::handle_disconnect()
    {
        std::lock_guard<std::mutex> lock(guard);

        // When disconnected, we need to move active subscriptions to the
        // list of subscriptions net yet subscribed.
        // Any subscription currently being subscribed should just be reset.

        reset_control_packet(subscribing);

        // As we're actively checking for duplicates when adding a subscription
        // we must first remove it.
        for (auto it = active_subscription.begin(); it != active_subscription.end(); )
        {
            auto copy = *it;
            it = active_subscription.erase(it);
            internal_subscribe(copy.first, copy.second);
        }

        // Reset any unsubscriptions
        reset_control_packet(unsubscribing);
    }

    void Subscription::receive(packet::SubAck& sub_ack, IMqttClient&)
    {
        std::lock_guard<std::mutex> lock(guard);
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
                    Log::debug(mqtt_log_tag, "Subscription of topic {} completed, QoS: {}", t.first, t.second);
                    active_subscription.emplace(t.first, t.second);
                }

                subscribing.erase(subscribing.begin());
            }
        }
    }

    void Subscription::receive(packet::Publish& publish, IMqttClient& mqtt)
    {
        std::lock_guard<std::mutex> lock(guard);

        // Note: It is a valid use case where a Publish packet is received
        // before a SubAck has been received for a subscription.

        if (publish.get_qos() == QoS::AT_MOST_ONCE)
        {
            forward_to_application(publish, mqtt);
        }
        else if (publish.get_qos() == QoS::AT_LEAST_ONCE)
        {
            packet::PubAck ack(publish.get_packet_identifier());
            mqtt.send_packet(ack);
            forward_to_application(publish, mqtt);
        }
        else if (publish.get_qos() == QoS::EXACTLY_ONCE)
        {
            // Do we know of a packet with this packet id already?
            auto known = receiving.find(publish.get_packet_identifier());

            if (known == receiving.end())
            {
                // Prepare to receive a PubRel
                InFlight<packet::Publish> flight(publish);
                flight.set_wait_packet(PacketType::PUBREL);
                flight.start_timer();
                receiving.insert(std::make_pair(publish.get_packet_identifier(), flight));
            }

            // Always send a PubRec message as an ack.
            packet::PubRec rec(publish.get_packet_identifier());
            mqtt.send_packet(rec);
        }
    }

    void Subscription::receive(packet::PubRel& pub_rel, IMqttClient& mqtt)
    {
        std::lock_guard<std::mutex> lock(guard);

        // Always respond with a PubComp
        packet::PubComp pub_comp(pub_rel.get_packet_identifier());
        mqtt.send_packet(pub_comp);

        auto found = receiving.find(pub_rel.get_packet_identifier());

        if (found != receiving.end())
        {
            // We may now forward the data to the application.
            forward_to_application((*found).second.get_packet(), mqtt);

            // We're done with the packet. Any new packet with the same ID will
            // be treated as a new publication.
            receiving.erase(found);
        }
    }

    void Subscription::receive(packet::UnsubAck& unsub_ack, IMqttClient&)
    {
        std::lock_guard<std::mutex> lock(guard);

        if (!unsubscribing.empty())
        {
            auto& front = unsubscribing.front();
            auto& packet = front.get_packet();

            if (packet.get_packet_identifier() == unsub_ack.get_packet_identifier())
            {
                std::vector<std::string> topics{};
                packet.get_topics(topics);

                for (auto& t : topics)
                {
                    Log::debug(mqtt_log_tag, "Unsubscription of topic {} completed", t);
                    active_subscription.erase(t);
                }

                unsubscribing.erase(unsubscribing.begin());
            }
        }
    }

    void Subscription::forward_to_application(const packet::Publish& publish, IMqttClient& mqtt) const
    {
        Log::debug(mqtt_log_tag, "Reception of QoS {} complete", publish.get_qos());

        // Grab the payload
        std::vector<uint8_t> payload;

        // Move data into our local vector.
        std::copy(std::make_move_iterator(publish.get_payload_cbegin()),
                  std::make_move_iterator(publish.get_payload_cend()),
                  std::back_inserter(payload));

        // Enqueue data to application. Can't avoid a copy here.
        const auto& app_queue = mqtt.get_application_queue().lock();

        if (app_queue)
        {
            app_queue->push(std::make_pair(publish.get_topic(), payload));
        }
    }
}
