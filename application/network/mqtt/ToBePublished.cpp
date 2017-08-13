//
// Created by permal on 8/12/17.
//

#include <smooth/application/network/mqtt/ToBePublished.h>
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

                void ToBePublished::publish(const std::string& topic, const uint8_t* data, int length, mqtt::QoS qos,
                                            bool retain)
                {
                    packet::Publish p(topic, data, length, qos, retain);
                    in_progress.push_back(InFlight(p));
                }


                void ToBePublished::publish_next(IMqtt& mqtt)
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
                            // Resend message based on timeout, set packet id and DUP flag
                            // based on original outgoing message.
                            if (flight.get_elapsed_time() > seconds(15))
                            {

                            }
                        }
                    }
                }

                void ToBePublished::receive(packet::PubAck& pub_ack, IMqtt& mqtt)
                {
                    auto first = in_progress.begin();
                    if (first != in_progress.end())
                    {
                        auto& in_flight = *first;
                        if (in_flight.get_packet().get_packet_identifier() == pub_ack.get_packet_identifier())
                        {
                            ESP_LOGV(tag, "QoS %d publish completed", in_flight.get_packet().get_qos());
                            in_progress.erase(in_progress.begin());
                        }
                    }
                }

                void ToBePublished::receive(packet::PubRec& pub_rec, IMqtt& mqtt)
                {
                    auto first = in_progress.begin();
                    if (first != in_progress.end())
                    {
                        auto& in_flight = *first;
                        if (in_flight.get_waiting_for() == PUBREC
                            && in_flight.get_packet().get_packet_identifier() == pub_rec.get_packet_identifier())
                        {
                            packet::PubRel pub_rel(in_flight.get_packet().get_packet_identifier());

                            if (mqtt.send_packet(pub_rel))
                            {
                                // Wait for PubComp and send PubRel
                                first->set_wait_packet(PUBCOMP);
                            }
                        }
                    }
                }

                void ToBePublished::receive(packet::PubComp& pub_rec, IMqtt& mqtt)
                {
                    auto first = in_progress.begin();
                    if (first != in_progress.end())
                    {
                        auto& in_flight = *first;

                        if (in_flight.get_waiting_for() == PUBCOMP
                            && in_flight.get_packet().get_packet_identifier() == pub_rec.get_packet_identifier())
                        {
                            ESP_LOGV(tag, "QoS %d publish completed", in_flight.get_packet().get_qos());
                            in_progress.erase(in_progress.begin());
                        }
                    }
                }

            }
        }
    }
}