//
// Created by permal on 8/12/17.
//

#include <smooth/application/network/mqtt/ToBePublished.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                void ToBePublished::publish(const std::string& topic, const uint8_t* data, int length, mqtt::QoS qos,
                                            bool retain)
                {
                    packet::Publish p(topic, data, length, qos, retain);

                    int publish_steps = 0;

                    if (qos == QoS::AT_LEAST_ONCE)
                    {
                        publish_steps = 1; // Wait for PubAck before considering the message published.
                    }
                    else if (qos == QoS::EXACTLY_ONCE)
                    {
                        publish_steps = 2; // Wait for PubRec, send PubRel, wait for PubRel
                    }


                    std::pair<packet::Publish, int> pair(p, publish_steps);
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
                            mqtt.send_packet(packet, std::chrono::seconds(2));
                            in_progress.erase(in_progress.begin());
                        }
                        else if (flight.get_step() == 0)
                        {
                            // Packet with QoS > AT_MOST_ONCE not yet sent first in queue

                            if (packet.get_qos() == QoS::AT_LEAST_ONCE)
                            {
                                // Send packet, wait for PubAck
                                if (mqtt.send_packet(packet, std::chrono::seconds(2)))
                                {
                                    flight.increase_step();
                                }
                            }
                            else if (packet.get_qos() == QoS::EXACTLY_ONCE)
                            {
                                // Send packet, wait for PubRec
                                if (mqtt.send_packet(packet, std::chrono::seconds(2)))
                                {
                                    flight.increase_step();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}