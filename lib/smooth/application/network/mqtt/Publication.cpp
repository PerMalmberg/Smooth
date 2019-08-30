// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <smooth/application/network/mqtt/Publication.h>
#include <smooth/application/network/mqtt/packet/PubRel.h>
#include <smooth/application/network/mqtt/packet/PubComp.h>
#include <smooth/application/network/mqtt/Logging.h>
#include <smooth/core/logging/log.h>

#ifdef ESP_PLATFORM
#include "sdkconfig.h"
#endif

using namespace smooth::core::logging;
using namespace std::chrono;

namespace smooth::application::network::mqtt
{
    Publication::Publication()
    {
        in_progress.reserve(CONFIG_SMOOTH_MAX_MQTT_OUTGOING_MESSAGES);
    }

    bool Publication::publish(const std::string& topic, const uint8_t* data, int length, mqtt::QoS qos,
                              bool retain)
    {
        std::lock_guard<std::mutex> lock(guard);
        bool res = in_progress.size() < CONFIG_SMOOTH_MAX_MQTT_OUTGOING_MESSAGES;

        if (res)
        {
            packet::Publish p(topic, data, length, qos, retain);
            in_progress.emplace_back(p);
        }

        return res;
    }

    void Publication::handle_disconnect()
    {
        std::lock_guard<std::mutex> lock(guard);

        // When a disconnection happens, any outgoing messages currently being timed must be reset
        // so that they don't cause a timeout before a resend of the package happens.
        if (!in_progress.empty())
        {
            auto& flight = in_progress.front();
            flight.zero_timer();
        }
    }

    void Publication::resend_outstanding_control_packet(IMqttClient& mqtt, bool clean_session)
    {
        std::lock_guard<std::mutex> lock(guard);

        // When a Client reconnects with CleanSession set to 0, both the Client and Server MUST re-send
        // any unacknowledged PUBLISH Packets (where QoS > 0) and PUBREL Packets using their original
        // Packet Identifiers [MQTT-4.4.0-1]. This is the only circumstance where a Client or Server is
        // REQUIRED to redeliver messages.

        if (!in_progress.empty())
        {
            auto& flight = in_progress.front();

            if (clean_session)
            {
                if (flight.get_waiting_for() == PacketType::PUBACK
                    || flight.get_waiting_for() == PacketType::PUBREC
                    || flight.get_waiting_for() == PacketType::PUBCOMP)
                {
                    // Drop message
                    in_progress.erase(in_progress.begin());
                }
            }
            else
            {
                auto& packet = flight.get_packet();

                if (flight.get_waiting_for() == PacketType::PUBACK)
                {
                    // Set dup flag and let normal procedure send the packet.
                    packet.set_dup_flag();
                    flight.zero_timer();
                    flight.set_wait_packet(PacketType::Reserved);
                }
                else if (flight.get_waiting_for() == PacketType::PUBREC)
                {
                    // Let normal procedure send the packet
                    flight.zero_timer();
                    flight.set_wait_packet(PacketType::Reserved);
                }
                else if (flight.get_waiting_for() == PacketType::PUBCOMP)
                {
                    packet::PubRel pub_rel(packet.get_packet_identifier());

                    // As this is running directly after a reconnect, and the TX buffer is cleared
                    // on disconnect, it is guaranteed that this message will fit in the buffer.
                    mqtt.send_packet(pub_rel);
                }
                else
                {
                    // Haven't gotten far enough to wait for PUBACK or PUBCOMP, reset.
                    packet.set_dup_flag();
                    flight.start_timer();
                    flight.set_wait_packet(PacketType::Reserved);
                }
            }
        }
    }

    void Publication::publish_next(IMqttClient& mqtt)
    {
        std::lock_guard<std::mutex> lock(guard);

        auto& flight = in_progress.front();
        auto& packet = flight.get_packet();

        if (!in_progress.empty())
        {
            if (packet.get_qos() == QoS::AT_MOST_ONCE)
            {
                // Fire and forget
                if (mqtt.send_packet(packet))
                {
                    Log::verbose(mqtt_log_tag,
                                 Format("QoS {1} publish completed", Int32(packet.get_qos())));
                    in_progress.erase(in_progress.begin());
                }
                else
                {
                    Log::error(mqtt_log_tag,
                               Format("Could not enqueue packet of QoS {1}",
                                      Int32(packet.get_qos())));
                }
            }
            else if (flight.get_waiting_for() == PacketType::Reserved)
            {
                // Packet with QoS > AT_MOST_ONCE and not yet sent first in queue.
                // We will only ever have a single active in-flight message

                if (packet.get_qos() == QoS::AT_LEAST_ONCE)
                {
                    // Send packet, wait for PubAck
                    if (mqtt.send_packet(packet))
                    {
                        flight.start_timer();
                        flight.set_wait_packet(PUBACK);
                    }
                    else
                    {
                        Log::error(mqtt_log_tag,
                                   Format("Could not enqueue packet of QoS {1}",
                                          Int32(packet.get_qos())));
                    }
                }
                else if (packet.get_qos() == QoS::EXACTLY_ONCE)
                {
                    // Send packet, wait for PubRec
                    if (mqtt.send_packet(packet))
                    {
                        flight.start_timer();
                        flight.set_wait_packet(PUBREC);
                    }
                    else
                    {
                        Log::error(mqtt_log_tag,
                                   Format("Could not enqueue packet of QoS {1}",
                                          Int32(packet.get_qos())));
                    }
                }

                // At this point, the message we're currently trying to send
                // will be left first in the vector.
            }
            else
            {
                // Still waiting for a reply...
                if (flight.get_elapsed_time() > seconds{ 5 })
                {
                    // Waited too long, force a disconnect.
                    Log::error(mqtt_log_tag,
                               Format(Str(
                                       "Too long since a reply was received to a publish message, forcing disconnect.")));

                    flight.stop_timer();
                    mqtt.force_disconnect();
                }
                else
                {
                    Log::debug(mqtt_log_tag,
                               Format("Waiting to send: {1}, QoS {1}, waiting for: {3}, timer: {4}ms",
                                      Str(packet.get_mqtt_type_as_string()),
                                      Int32(packet.get_qos()),
                                      Int32(flight.get_waiting_for()),
                                      Int64(flight.get_elapsed_time().count())));
                }
            }
        }
    }

    void Publication::receive(packet::PubAck& pub_ack, IMqttClient &)
    {
        std::lock_guard<std::mutex> lock(guard);
        auto first = in_progress.begin();

        if (first != in_progress.end())
        {
            auto& flight = *first;

            if (flight.get_packet().get_packet_identifier() == pub_ack.get_packet_identifier())
            {
                Log::verbose(mqtt_log_tag,
                             Format("QoS {1} publish completed", Int32(flight.get_packet().get_qos())));
                in_progress.erase(in_progress.begin());
            }
        }
    }

    void Publication::receive(packet::PubRec& pub_rec, IMqttClient& mqtt)
    {
        std::lock_guard<std::mutex> lock(guard);
        auto first = in_progress.begin();

        if (first != in_progress.end())
        {
            auto& flight = *first;

            if (flight.get_waiting_for() == PUBREC
                && flight.get_packet().get_packet_identifier() == pub_rec.get_packet_identifier())
            {
                flight.start_timer();

                packet::PubRel pub_rel(flight.get_packet().get_packet_identifier());

                if (mqtt.send_packet(pub_rel))
                {
                    // Wait for PubComp and send PubRel
                    first->set_wait_packet(PUBCOMP);
                }
                else
                {
                    // Couldn't send, this will result in a timeout and reconnect with subsequent resend.
                }
            }
        }
    }

    void Publication::receive(packet::PubComp& pub_rec, IMqttClient &)
    {
        std::lock_guard<std::mutex> lock(guard);
        auto first = in_progress.begin();

        if (first != in_progress.end())
        {
            auto& flight = *first;

            if (flight.get_waiting_for() == PUBCOMP
                && flight.get_packet().get_packet_identifier() == pub_rec.get_packet_identifier())
            {
                Log::verbose(mqtt_log_tag,
                             Format("QoS {1} publish completed",
                                    Int32(flight.get_packet().get_qos())));
                in_progress.erase(in_progress.begin());
            }
        }
    }
}
