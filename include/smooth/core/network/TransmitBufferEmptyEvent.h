//
// Created by permal on 7/5/17.
//

#pragma once

#include <smooth/core/util/CircularBuffer.h>
#include <smooth/core/network/ISocket.h>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            class TransmitBufferEmptyEvent
            {
                public:
                    TransmitBufferEmptyEvent() = default;

                    TransmitBufferEmptyEvent(smooth::core::network::ISocket* socket) : s(socket)
                    {
                    }

                    smooth::core::network::ISocket* get_socket() const
                    {
                        return s;
                    }

                private:
                    smooth::core::network::ISocket* s;
            };
        }
    }
}