//
// Created by permal on 7/5/17.
//

#pragma once

#include <smooth/util/CircularBuffer.h>

namespace smooth
{
    namespace network
    {
        class TransmitBufferEmpty
        {
            public:
                TransmitBufferEmpty() = default;

                TransmitBufferEmpty(smooth::network::ISocket* socket) : s(socket)
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