#include <cstdint>
#include "HTTPPacket.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

namespace secure_socket_test
{
    int HTTPPacket::get_wanted_amount()
    {
        // Return the number of bytes available in the buffer
        return state == State::reading_headers ? static_cast<int>(data.size() - bytes_received) : content_length -
                                                                                                  bytes_received;
    }

    void HTTPPacket::data_received(int length)
    {
        bytes_received += length;

        if (state == State::reading_headers)
        {
            if (ends_with_two_crlf())
            {
                // End of header
                parse_headers();
                state = State::reading_content;
                bytes_received = 0;
                data.clear();
                content_length = headers["Content-Length"].empty() ? 0 : std::stoi(headers["Content-Length"]);
                data.reserve(static_cast<size_t>(content_length));
            }
            else
            {
                prepare_for_one_more_byte();
            }
        }
    }

    uint8_t *HTTPPacket::get_write_pos()
    {
        return &data[bytes_received];
    }

    bool HTTPPacket::is_complete()
    {
        return state != State::reading_headers && bytes_received >= content_length;
    }

    bool HTTPPacket::is_error()
    {
        return false;
    }

    int HTTPPacket::get_send_length()
    {
        return static_cast<int>(data.size());
    }

    const uint8_t *HTTPPacket::get_data()
    {
        return data.data();
    }

    HTTPPacket::HTTPPacket(const std::string& data)
    {
        std::copy(data.begin(), data.end(), std::back_inserter(this->data));
    }

    bool HTTPPacket::ends_with_crlf() const
    {
        // At least one character and \r\n
        return data.size() > 2
               && data[data.size() - 2] == '\r'
               && data[data.size() - 1] == '\n';
    }

    bool HTTPPacket::ends_with_two_crlf() const
    {
        // At least one character and \r\n\r\n
        return data.size() > 4
               && data[data.size() - 4] == '\r'
               && data[data.size() - 3] == '\n'
               && data[data.size() - 2] == '\r'
               && data[data.size() - 1] == '\n';
    }


    void HTTPPacket::prepare_for_one_byte()
    {
        bytes_received = 0;
        data.clear();
        data.push_back(0);
    }

    void HTTPPacket::prepare_for_one_more_byte()
    {
        data.push_back(0);
    }

    void HTTPPacket::parse_headers()
    {
        data.push_back(0);
        std::stringstream ss;

        data.erase(std::remove_if(data.begin(), data.end(), [](const auto& b) { return b == '\n'; }), data.end());

        for (char c : data)
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
                    status_line = s;
                }
                else
                {
                    if (std::distance(colon, s.end()) > 2)
                    {
                        headers[{s.begin(), colon}] = {colon + 2, s.end()};
                    }
                }
            }
        }
    }
}