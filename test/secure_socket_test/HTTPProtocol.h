#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <smooth/core/network/IPacketAssembly.h>
#include <smooth/core/network/IPacketDisassembly.h>
#include "HTTPPacket.h"

// Please note that this implementation is rather naive. It does only the the minimum required things to
// successfully download a file. Further, it is susceptible to DoS attacks in the form of large headers
// since it does not do any size checking on them.

namespace secure_socket_test
{
    template<int MaxPacketSize = 100>
    class HTTPProtocol
            : public smooth::core::network::IPacketAssembly<HTTPProtocol<MaxPacketSize>, HTTPPacket>
    {
        public:
            using packet_type = HTTPPacket;

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
            int content_bytes_received = 0;
            const std::regex response_line{"(.+) (\\d+) (.+)"};

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
            res = content_length - packet.get_bytes_received();
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

                parse_headers(packet);
                state = State::reading_content;

                try
                {
                    content_length = packet.headers()["Content-Length"].empty() ? 0 : std::stoi(
                            packet.headers()["Content-Length"]);
                    content_bytes_received = 0;
                }
                catch(...)
                {
                    content_length = 0;
                }

                if(content_length > 0)
                {
                    packet.set_continued();
                }
            }
        }
        else
        {
            content_bytes_received += length;

            if(packet.empty_space() == 0)
            {
                packet.set_continuation();
                if(content_bytes_received < content_length)
                {
                    packet.set_continued();
                }
            }
        }
    }

    template<int MaxPacketSize>
    uint8_t* HTTPProtocol<MaxPacketSize>::get_write_pos(HTTPPacket& packet)
    {
        return &packet.data()[static_cast<std::vector<uint8_t>::size_type>(packet.get_bytes_received())];
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

        for (auto c : packet.data())
        {
            if(c != '\n')
            {
                ss << static_cast<char>(c);
            }
        }

        std::string s;
        while (std::getline(ss, s, '\r'))
        {
            if (!s.empty())
            {
                auto colon = std::find(s.begin(), s.end(), ':');
                if (colon == s.end() && !s.empty())
                {
                    std::smatch m;
                    if(std::regex_match(s, m, response_line))
                    {
                        auto status = stoi(m[2].str());
                        packet.set_status(status);
                    }
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
        packet.set_size(get_wanted_amount(packet));
    }

}