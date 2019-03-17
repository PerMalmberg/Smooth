#include <cstdint>
#include "HTTPProtocol.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

namespace secure_socket_test
{
    int HTTPProtocol::get_wanted_amount(HTTPPacket& packet)
    {
        // Return the number of bytes available in the buffer
        return state == State::reading_headers ? static_cast<int>(packet.content.size() - bytes_received) : content_length -
                                                                                                  bytes_received;
    }

    void HTTPProtocol::data_received(HTTPPacket& packet, int length)
    {
        bytes_received += length;

        if (state == State::reading_headers)
        {
            if (ends_with_two_crlf(packet))
            {
                // End of header
                parse_headers(packet);
                state = State::reading_content;
                bytes_received = 0;
                content_length = packet.headers["Content-Length"].empty() ? 0 : std::stoi(packet.headers["Content-Length"]);
                packet.content.reserve(static_cast<size_t>(content_length));
            }
            else
            {
                prepare_for_one_more_byte(packet);
            }
        }
    }

    uint8_t *HTTPProtocol::get_write_pos(HTTPPacket& packet)
    {
        return &packet.content[bytes_received];
    }

    bool HTTPProtocol::is_complete()
    {
        return state != State::reading_headers && bytes_received >= content_length;
    }

    bool HTTPProtocol::is_error()
    {
        return false;
    }

    bool HTTPProtocol::ends_with_two_crlf(const HTTPPacket& packet) const
    {
        // At least one character and \r\n\r\n
        return packet.content.size() > 4
               && packet.content[packet.content.size() - 4] == '\r'
               && packet.content[packet.content.size() - 3] == '\n'
               && packet.content[packet.content.size() - 2] == '\r'
               && packet.content[packet.content.size() - 1] == '\n';
    }


    void HTTPProtocol::prepare_for_one_more_byte(HTTPPacket& packet)
    {
        packet.content.push_back(0);
    }

    void HTTPProtocol::parse_headers(HTTPPacket& packet)
    {
        packet.content.push_back(0);
        std::stringstream ss;

        packet.content.erase(std::remove_if(packet.content.begin(), packet.content.end(), [](const auto& b) { return b == '\n'; }), packet.content.end());

        for (char c : packet.content)
        {
            ss << c;
        }

        std::string s;
        while (std::getline(ss, s, '\r'))
        {
            s.erase(std::remove_if(s.begin(), s.end(), [](const char c) { return c == '\r' || c == 0;}), s.end());
            if (!s.empty())
            {
                auto colon = std::find(s.begin(), s.end(), ':');
                if (colon == s.end() && !s.empty())
                {
                    packet.status_line = s;
                }
                else
                {
                    if (std::distance(colon, s.end()) > 2)
                    {
                        packet.headers[{s.begin(), colon}] = {colon + 2, s.end()};
                    }
                }
            }
        }

        packet.content.clear();
    }

    void HTTPProtocol::packet_consumed()
    {
        bytes_received = 0;
    }
}