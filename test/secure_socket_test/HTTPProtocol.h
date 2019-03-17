#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <smooth/core/network/IPacketAssembly.h>
#include <smooth/core/network/IPacketDisassembly.h>
#include "HTTPPacket.h"

namespace secure_socket_test
{
    class HTTPProtocol
            : public smooth::core::network::IPacketAssembly<HTTPProtocol, HTTPPacket>
    {
        public:
            using packet_type = HTTPPacket;

            explicit HTTPProtocol(HTTPPacket& working_packet)
            : packet(working_packet)
            {
            }


            /// Must return the number of bytes the packet wants to fill
            /// its internal buffer, e.g. header, checksum etc. Returned
            /// value will differ depending on how much data already has been provided.
            /// \param packet The packet being assembled
            /// \return Number of bytes wanted
            int get_wanted_amount(HTTPPacket& packet) override;

            /// Used by the underlying framework to notify the packet that {length} bytes
            /// has been written to the buffer pointed to by get_write_pos().
            /// During the call to this method the packet should do whatever it needs to
            /// evaluate if it needs more data or if it is complete.
            /// \param packet The packet being assembled
            /// \param length Number of bytes received
            void data_received(HTTPPacket& packet, int length) override;

            /// Must return the current write position of the internal buffer.
            /// Must point to a buffer than can accept the number of bytes returned by
            /// get_wanted_amount().
            /// \param packet The packet being assembled
            /// \return Write position
            uint8_t* get_write_pos(HTTPPacket& packet) override;

            /// Must return true when the packet has received all data it needs
            /// to fully assemble.
            /// \return true or false
            bool is_complete() override;

            /// Must return true whenever the packet is unable to correctly assemble
            /// based on received data.
            /// \return true or false
            bool is_error() override;

            void packet_consumed() override;

        private:
            bool ends_with_two_crlf(const HTTPPacket& packet) const;

            void prepare_for_one_more_byte(HTTPPacket& packet);

            void parse_headers(HTTPPacket& packet);

            enum class State
            {
                    reading_headers,
                    reading_content
            };

            int bytes_received = 0;
            int content_length = 0;
            HTTPPacket& packet;

            State state = State::reading_headers;
    };

}