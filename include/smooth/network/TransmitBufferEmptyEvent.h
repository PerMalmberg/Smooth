//
// Created by permal on 7/5/17.
//

#pragma once

#include <smooth/util/CircularBuffer.h>
#include <smooth/network/ISocket.h>

namespace smooth
{
    namespace network
    {
        class TransmitBufferEmptyEvent
        {
            public:
                TransmitBufferEmptyEvent() = default;

                TransmitBufferEmptyEvent(smooth::network::ISocket* socket) : s(socket)
                {
                }

                smooth::network::ISocket* get_socket() const
                {
                    return s;
                }

            private:
                smooth::network::ISocket* s;
        };
    }
}