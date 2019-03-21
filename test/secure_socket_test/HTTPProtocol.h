#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <smooth/core/network/IPacketAssembly.h>
#include <smooth/core/network/IPacketDisassembly.h>
#include "HTTPPacket.h"

namespace secure_socket_test
{
    template<int MaxPacketSize = 100>
    class HTTPProtocol
            : public smooth::core::network::IPacketAssembly<HTTPProtocol<MaxPacketSize>, HTTPPacket>
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
            void parse_headers(HTTPPacket& packet);

            enum class State
            {
                    reading_headers,
                    reading_content
            };

            int content_length = 0;
            HTTPPacket& packet;

            State state = State::reading_headers;
    };

    template<int MaxPacketSize>
    int HTTPProtocol<MaxPacketSize>::get_wanted_amount(HTTPPacket& packet)
    {
        int res;

        // Return the number of bytes available in the buffer
        if(state == State::reading_headers)
        {
            if (packet.empty_space() == 0)
            {
                // Make room for one byte
                packet.increase_size(1);
            }

            res = 1;
        }
        else
        {
            res = std::min(MaxPacketSize, content_length - packet.get_bytes_received());
            packet.set_size(res);
        }

        return res;
    }

    template<int MaxPacketSize>
    void HTTPProtocol<MaxPacketSize>::data_received(HTTPPacket& packet, int length)
    {
        packet.data_received(length);

        if (state == State::reading_headers)
        {
            if (packet.ends_with_two_crlf())
            {
                // End of header

                // Pretend we got another byte and write a terminating 0.
                packet.append_null();

                parse_headers(packet);
                state = State::reading_content;

                try
                {
                    content_length = packet.headers()["Content-Length"].empty() ? 0 : std::stoi(
                            packet.headers()["Content-Length"]);
                }
                catch(...)
                {
                    content_length = 0;
                }

                if(content_length > 0)
                {
                    // More data to receive so this packet will be followed by at least one more.
                    packet.set_continued();
                }
            }
        }
        else
        {
            if(packet.empty_space() == 0)
            {
                packet.set_continuation();
                if(packet.get_bytes_received() < content_length)
                {
                    packet.set_continued();
                }
            }
        }
    }

    template<int MaxPacketSize>
    uint8_t* HTTPProtocol<MaxPacketSize>::get_write_pos(HTTPPacket& packet)
    {
        return &packet.data()[packet.get_bytes_received()];
    }

    template<int MaxPacketSize>
    bool HTTPProtocol<MaxPacketSize>::is_complete()
    {
        return state != State::reading_headers && packet.empty_space() == 0;
    }

    template<int MaxPacketSize>
    bool HTTPProtocol<MaxPacketSize>::is_error()
    {
        return false;
    }

    template<int MaxPacketSize>
    void HTTPProtocol<MaxPacketSize>::parse_headers(HTTPPacket& packet)
    {
        std::stringstream ss;

        for (char c : packet.data())
        {
            if(c != '\n')
            {
                ss << c;
            }
        }

        std::string s;
        while (std::getline(ss, s, '\r'))
        {
            s.erase(std::remove_if(s.begin(), s.end(), [](const char c) { return c == '\r' || c == 0; }), s.end());
            if (!s.empty())
            {
                auto colon = std::find(s.begin(), s.end(), ':');
                if (colon == s.end() && !s.empty())
                {
                    auto a = s;

                }
                else
                {
                    if (std::distance(colon, s.end()) > 2)
                    {
                        packet.headers()[{s.begin(), colon}] = {colon + 2, s.end()};
                    }
                }
            }
        }

        packet.clear();
    }

    template<int MaxPacketSize>
    void HTTPProtocol<MaxPacketSize>::packet_consumed()
    {
        packet.clear();
    }

}