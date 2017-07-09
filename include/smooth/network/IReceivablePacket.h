//
// Created by permal on 7/9/17.
//

#pragma once

namespace smooth
{
    namespace network
    {
        class IReceivablePacket
        {
            public:
                // Must return the number of bytes the packet wants to fill
                // its internal buffer, e.g. header, checksum etc. Returned
                // value will differ depending on how much data already has been provided.
                virtual int get_wanted_amount() = 0;

                // Used by the underlying framework to notify the packet that {length} bytes
                // has been written to the buffer pointed to by get_write_pos().
                // During the call to this method the packet should do whatever it needs to
                // evaluate if it needs more data or if it is complete.
                virtual void data_received(int length) = 0;

                // Must return the current write position of the internal buffer.
                // Must point to a buffer than can accept the number of bytes returned by
                // get_wanted_amount().
                virtual uint8_t* get_write_pos() = 0;

                // Must return true when the packet has received all data it needs
                // to fully assemble.
                virtual bool is_complete() = 0;

                // Must return true whenever the packet is unable to correctly assemble
                // based on received data.
                virtual bool is_error() = 0;

                virtual ~IReceivablePacket(){}
        };
    }
}
