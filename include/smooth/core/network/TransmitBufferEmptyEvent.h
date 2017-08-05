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

                    TransmitBufferEmptyEvent(std::shared_ptr<smooth::core::network::ISocket> socket) : s(socket)
                    {
                    }

                    std::shared_ptr<smooth::core::network::ISocket> get_socket() const
                    {
                        return s;
                    }

                private:
                    std::shared_ptr<smooth::core::network::ISocket> s;
            };
        }
    }
}