//
// Created by permal on 7/31/17.
//

#pragma once

#include <chrono>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
               class ToBePublished;


                class IMqtt
                {
                    public:
                        virtual ~IMqtt()
                        {
                        }

                        virtual const std::string& get_client_id() const = 0;
                        virtual const std::chrono::seconds get_keep_alive() const = 0;
                        virtual void start_reconnect() = 0;
                        virtual void reconnect() = 0;
                        virtual bool is_auto_reconnect() const = 0;
                        virtual void set_keep_alive_timer(std::chrono::seconds interval) = 0;
                        virtual bool send_packet(packet::MQTTPacket& packet) = 0;
                        virtual ToBePublished& get_to_be_published() = 0;
                };

            }
        }
    }
}