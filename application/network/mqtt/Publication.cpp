//
// Created by permal on 8/12/17.
//

#include <smooth/application/network/mqtt/Publication.h>
#include <smooth/application/network/mqtt/packet/PubRel.h>
#include <smooth/application/network/mqtt/packet/PubComp.h>
#include "esp_log.h"

using namespace std::chrono;

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                static const char* tag = "MQTT-Publish";

                void Publication::publish(const std::string& topic, const uint8_t* data, int length, mqtt::QoS qos,
                                            bool retain)
                {
                    packet::Publish p(topic, data, length, qos, retain);
                    in_progress.push_back(InFlight<packet::Publish>(p));
                }

                void Publication::resend_outstanding_control_packet(IMqtt& mqtt)
                {
                    // When a Client reconnects with CleanSession set to 0, both the Client and Server MUST re-send
                    // any unacknowledged PUBLISH Packets (where QoS > 0) and PUBREL Packets using their original
                    // Packet Identifiers [MQTT-4.4.0-1]. This is the only circumstance where a Client or Server is
                    // REQUIRED to redeliver messages.

                    if (in_progress.size() > 0)
                    {
                        auto& flight = in_progress.front();
                        auto& packet = flight.get_packet();

                        if (flight.get_waiting_for() == PacketType::PUBACK)
                        {
                            // Set dup flag and let normal procedure send the packet.
                            packet.set_dup_flag();
                            flight.set_wait_packet(PacketType::Reserved);
                        }
                        else if (flight.get_waiting_for() == PacketType::PUBCOMP)
                        {
                            packet::PubRel pub_rel(flight.get_packet().get_packet_identifier());

                            // As this is running directly after a reconnect, and the TX buffer is cleared
                            // on disconnect, it is guaranteed that this message will fit on the buffer.
                            mqtt.send_packet(pub_rel);
                        }

                    }
                }

                void Publication::publish_next(IMqtt& mqtt)
                {
                    if (in_progress.size() > 0)
                    {
                        auto& flight = in_progress.front();
                        auto& packet = flight.get_packet();

                        if (packet.get_qos() == QoS::AT_MOST_ONCE)
                        {
                            // Fire and forget
                            if (mqtt.send_packet(packet))
                            {
                                ESP_LOGV(tag, "QoS %d publish completed", packet.get_qos());
                                in_progress.erase(in_progress.begin());
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
                            }
                            else if (packet.get_qos() == QoS::EXACTLY_ONCE)
                            {
                                // Send packet, wait for PubRec
                                if (mqtt.send_packet(packet))
                                {
                                    flight.start_timer();
                                    flight.set_wait_packet(PUBREC);
                                }
                            }

                            // At this point, the message we're currently trying to send
                            // will be left first in the vector.
                        }
                        else
                        {
                            // Still waiting for a reply...
                            if (flight.get_elapsed_time() > seconds(15))
                            {
                                // Waited too long, force a reconnect.
                                ESP_LOGE("MQTT", "Too long since a reply was received to a published message, forcing reconnect.");
                                flight.stop_timer();
                                mqtt.reconnect();
                            }
                        }
                    }
                }

                void Publication::receive(packet::PubAck& pub_ack, IMqtt& mqtt)
                {
                    auto first = in_progress.begin();
                    if (first != in_progress.end())
                    {
                        auto& flight = *first;
                        if (flight.get_packet().get_packet_identifier() == pub_ack.get_packet_identifier())
                        {
                            ESP_LOGV(tag, "QoS %d publish completed", flight.get_packet().get_qos());
                            in_progress.erase(in_progress.begin());
                        }
                    }
                }

                void Publication::receive(packet::PubRec& pub_rec, IMqtt& mqtt)
                {
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

                void Publication::receive(packet::PubComp& pub_rec, IMqtt& mqtt)
                {
                    auto first = in_progress.begin();
                    if (first != in_progress.end())
                    {
                        auto& flight = *first;

                        if (flight.get_waiting_for() == PUBCOMP
                            && flight.get_packet().get_packet_identifier() == pub_rec.get_packet_identifier())
                        {
                            ESP_LOGV(tag, "QoS %d publish completed", flight.get_packet().get_qos());
                            in_progress.erase(in_progress.begin());
                        }
                    }
                }

            }
        }
    }
}