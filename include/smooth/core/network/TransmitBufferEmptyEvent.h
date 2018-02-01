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
            /// En event sent to the application when all outgoing packets have been sent.
            class TransmitBufferEmptyEvent
            {
                public:
                    TransmitBufferEmptyEvent() = default;

                    explicit TransmitBufferEmptyEvent(std::shared_ptr<smooth::core::network::ISocket> socket) : s(std::move(socket))
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