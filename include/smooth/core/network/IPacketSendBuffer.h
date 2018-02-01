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
            /// Interface for packet send buffers
            /// \tparam PacketType
            template<typename PacketType>
            class IPacketSendBuffer
            {
                public:
                    virtual ~IPacketSendBuffer() = default;

                    /// Returns a value indicating if a packet is currently being sent.
                    /// \return true or false.
                    virtual bool is_in_progress() = 0;
                    /// Returns the start of the data to be sent.
                    /// \return A pointer to the first byte of the data to send.
                    virtual const uint8_t* get_data_to_send() = 0;
                    /// Gets the number of bytes to be sent.
                    /// \return The number of bytes remaining to be sent.
                    virtual size_t get_remaining_data_length() = 0;
                    /// Called when the specified amount of data has been sent.
                    /// \param length The number of bytes that has been sent.
                    virtual void data_has_been_sent(size_t length) = 0;
                    /// Perpares the next packet to be sent.
                    virtual void prepare_next_packet() = 0;
                    /// Puts an item into the buffer to be sent.
                    /// \return true if the item could be queued, otherwise false.
                    virtual bool put(const PacketType& item) = 0;
                    /// Clears the buffer.
                    virtual void clear() = 0;
                    /// Returns an item indicating if the buffer is empty.
                    /// \return true or false.
                    virtual bool is_empty() = 0;
            };
        }
    }
}