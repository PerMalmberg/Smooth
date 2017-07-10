//
// Created by permal on 7/9/17.
//

#pragma once

namespace smooth
{
    namespace network
    {
        template<typename T>
        class IPacketSendBuffer
        {
            public:
                virtual ~IPacketSendBuffer()
                {
                }

                virtual bool is_in_progress() = 0;
                virtual const uint8_t* get_data_to_send() = 0;
                virtual int get_remaining_data_length() = 0;
                virtual void data_has_been_sent(int length) = 0;
                virtual void prepare_next_packet() = 0;
                virtual void put(const T& item) = 0;
                virtual void clear() = 0;
                virtual bool is_empty() = 0;
        };
    }
}
