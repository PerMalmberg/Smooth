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
            /// Interface for packets that can be disassembled into a series of bytes
            class IPacketDisassembly
            {
                public:
                    /// Must return the total amount of bytes to send
                    /// \return Number of bytes to send
                    virtual size_t get_send_length() = 0;
                    /// Must return a pointer to the data to be sent.
                    /// \return The read position
                    virtual const uint8_t* get_data() = 0;

                    virtual ~IPacketDisassembly() = default;
            };
        }
    }
}