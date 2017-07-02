//
// Created by permal on 7/2/17.
//

#pragma once

namespace smooth
{
    namespace network
    {
        class ISocket
        {
            public:
                virtual void readable() = 0;
                virtual void writable() = 0;
                virtual bool has_data_to_transmit() = 0;
                virtual void internal_start() = 0;
                virtual bool is_started() = 0;
                virtual void stop() = 0;
                virtual int get_socket_id() = 0;
        };
    }
}