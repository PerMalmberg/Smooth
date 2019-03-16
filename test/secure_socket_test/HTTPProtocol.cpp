#include <cstdint>
#include "HTTPProtocol.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

namespace secure_socket_test
{
    int HTTPProtocol::get_wanted_amount()
    {
        // Return the number of bytes available in the buffer
        return state == State::reading_headers ? static_cast<int>(data.size() - bytes_received) : content_length -
                                                                                                  bytes_received;
    }

    void HTTPProtocol::data_received(int length)
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
                content_length = headers["Content-Length"].empty() ? 0 : std::stoi(headers["Content-Length"]);
                data.reserve(static_cast<size_t>(content_length));
            }
            else
            {
                prepare_for_one_more_byte();
            }
        }
    }

    uint8_t *HTTPProtocol::get_write_pos()
    {
        return &data[bytes_received];
    }

    bool HTTPProtocol::is_complete()
    {
        return state != State::reading_headers && bytes_received >= content_length;
    }

    bool HTTPProtocol::is_error()
    {
        return false;
    }

    int HTTPProtocol::get_send_length()
    {
        return static_cast<int>(data.size());
    }

    const uint8_t *HTTPProtocol::get_data()
    {
        return data.data();
    }

    HTTPProtocol::HTTPProtocol(const std::string& data)
    {
        std::copy(data.begin(), data.end(), std::back_inserter(this->data));
    }

    bool HTTPProtocol::ends_with_crlf() const
    {
        // At least one character and \r\n
        return data.size() > 2
               && data[data.size() - 2] == '\r'
               && data[data.size() - 1] == '\n';
    }

    bool HTTPProtocol::ends_with_two_crlf() const
    {
        // At least one character and \r\n\r\n
        return data.size() > 4
               && data[data.size() - 4] == '\r'
               && data[data.size() - 3] == '\n'
               && data[data.size() - 2] == '\r'
               && data[data.size() - 1] == '\n';
    }


    void HTTPProtocol::prepare_for_one_byte()
    {
        bytes_received = 0;
        data.clear();
        data.push_back(0);
    }

    void HTTPProtocol::prepare_for_one_more_byte()
    {
        data.push_back(0);
    }

    void HTTPProtocol::parse_headers()
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