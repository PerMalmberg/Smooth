#pragma once

#include <chrono>

namespace smooth::core::network
{
    class ISocketBackOff
    {
        public:
            virtual ~ISocketBackOff() = default;
            // When called, tells the socket dispatcher to hold off all events for the
            // given socket id for the specified duration.
            virtual void back_off(int socket_id, std::chrono::milliseconds duration) = 0;
    };
}