#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <smooth/core/network/IPacketAssembly.h>
#include <smooth/core/network/IPacketDisassembly.h>

namespace secure_socket_test
{
    class HTTPPacket
            : public smooth::core::network::IPacketAssembly,
              public smooth::core::network::IPacketDisassembly
    {
        public:
            HTTPPacket()
                    : data(1)
            {}

            ~HTTPPacket() override = default;

            explicit HTTPPacket(const std::string& data);

            /// Must return the number of bytes the packet wants to fill
            /// its internal buffer, e.g. header, checksum etc. Returned
            /// value will differ depending on how much data already has been provided.
            /// \return Number of bytes wanted
            int get_wanted_amount() override;

            /// Used by the underlying framework to notify the packet that {length} bytes
            /// has been written to the buffer pointed to by get_write_pos().
            /// During the call to this method the packet should do whatever it needs to
            /// evaluate if it needs more data or if it is complete.
            /// \param length Number of bytes received
            void data_received(int length) override;

            /// Must return the current write position of the internal buffer.
            /// Must point to a buffer than can accept the number of bytes returned by
            /// get_wanted_amount().
            /// \return Write position
            uint8_t *get_write_pos() override;

            /// Must return true when the packet has received all data it needs
            /// to fully assemble.
            /// \return true or false
            bool is_complete() override;

            /// Must return true whenever the packet is unable to correctly assemble
            /// based on received data.
            /// \return true or false
            bool is_error() override;

            /// Must return the total amount of bytes to send
            /// \return Number of bytes to send
            int get_send_length() override;

            /// Must return a pointer to the data to be sent.
            /// \return The read position
            const uint8_t *get_data() override;

            const std::unordered_map<std::string, std::string> get_headers() const { return headers; }
            const std::string get_status_line() const { return status_line; }

        private:
            bool ends_with_crlf() const;

            bool ends_with_two_crlf() const;

            void prepare_for_one_byte();

            void prepare_for_one_more_byte();

            void parse_headers();

            enum class State
            {
                    reading_headers,
                    reading_content
            };
            std::vector<uint8_t> data;
            std::string status_line{};
            int bytes_received = 0;
            int content_length = 0;

            State state = State::reading_headers;

            std::unordered_map<std::string, std::string> headers{};
    };

}