//
// Created by permal on 7/9/17.
//

#pragma once

namespace smooth
{
    namespace core
    {
        namespace network
        {
            class ISendablePacket
            {
                public:
                    // Must return the total amount of bytes to send
                    virtual int get_send_length() = 0;
                    // Must return a pointer to the data to be sent.
                    virtual const uint8_t* get_data() = 0;

                    virtual ~ISendablePacket()
                    {
                    }
            };
        }
    }
}