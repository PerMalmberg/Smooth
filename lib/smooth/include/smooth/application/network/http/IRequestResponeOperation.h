#pragma once

#include <smooth/core/network/BufferContainer.h>
#include "HTTPProtocol.h"

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                enum class SendStatus
                {
                        AllSent
                        MoreToSend,
                };

                class IRequestResponseOperation
                {
                    public:
                        virtual SendStatus send_next_packet(smooth::core::network::IPacketSender<HTTPProtocol> sender);
                };
            }
        }
    }
}