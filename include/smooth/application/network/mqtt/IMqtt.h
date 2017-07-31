//
// Created by permal on 7/31/17.
//

#pragma once

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {

                class IMqtt
                {
                    public:
                        virtual ~IMqtt()
                        {
                        }

                        virtual const std::string& get_client_id() const = 0;
                        virtual void send_packet(packet::MQTTPacket& packet, std::chrono::milliseconds timeout) = 0;
                };

            }
        }
    }
}