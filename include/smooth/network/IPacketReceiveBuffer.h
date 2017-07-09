//
// Created by permal on 7/9/17.
//

#pragma once

namespace smooth
{
    namespace network
    {
        template<typename T>
        class IPacketReceiveBuffer
        {
            public:
                virtual bool is_full() = 0;
                virtual int amount_wanted() = 0;
                virtual char* get_write_pos() = 0;
                virtual void data_received(int length) = 0;
                virtual bool is_packet_complete() = 0;
                virtual bool get(T& target) = 0;
                virtual void clear() = 0;
                virtual bool is_in_progress() = 0;
                virtual void prepare_new_packet() = 0;
                virtual bool is_error() = 0;
        };
    }
}
