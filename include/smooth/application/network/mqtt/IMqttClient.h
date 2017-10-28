//
// Created by permal on 7/31/17.
//

#pragma once

#include <chrono>
#include <smooth/core/ipc/TaskEventQueue.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                class Publication;

                class Subscription;

                class IMqttClient
                {
                    public:
                        virtual ~IMqttClient() = default;

                        virtual const std::string& get_client_id() const = 0;
                        virtual const std::chrono::seconds get_keep_alive() const = 0;
                        virtual void start_reconnect() = 0;
                        virtual void reconnect() = 0;
                        virtual bool is_auto_reconnect() const = 0;
                        virtual void disconnect() = 0;
                        virtual void set_keep_alive_timer(std::chrono::seconds interval) = 0;
                        virtual bool send_packet(packet::MQTTPacket& packet) = 0;
                        virtual Publication& get_publication() = 0;
                        virtual Subscription& get_subscription() = 0;
                        virtual core::ipc::TaskEventQueue<std::pair<std::string, std::vector<uint8_t>>>&
                                                    get_application_queue() = 0;
                };

            }
        }
    }
}