/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <algorithm>
#include "smooth/core/util/string_util.h"
#include "smooth/application/network/http/regular/HTTPHeaderDef.h"
#include "smooth/application/network/http/regular/RegularHTTPProtocol.h"
#include "smooth/application/network/http/regular/responses/ErrorResponse.h"

namespace smooth::application::network::http
{
    using namespace smooth::application::network::http::regular;
    using namespace smooth::core;

    int RegularHTTPProtocol::get_wanted_amount(HTTPPacket& packet)
    {
        int amount_to_request;

        // Return the number of bytes wanted to fill the packet
        if (state == State::reading_headers)
        {
            // How much left until we reach max header size?
            amount_to_request = max_header_size - total_bytes_received;

            // Make sure there is room for what he have received and what we ask for.
            packet.expand_by(amount_to_request);
        }
        else
        {
            // Never ask for more than content_chunk_size
            auto remaining_of_content = incoming_content_length - total_content_bytes_received;
            amount_to_request = std::min(content_chunk_size, remaining_of_content);

            packet.expand_by(amount_to_request);
        }

        return amount_to_request;
    }

    void RegularHTTPProtocol::data_received(HTTPPacket& packet, int length)
    {
        total_bytes_received += length;

        if (state == State::reading_headers)
        {
            const auto end_of_header = packet.find_header_ending();

            if (end_of_header != packet.data().end())
            {
                // End of header found
                state = State::reading_content;
                actual_header_size = consume_headers(packet, end_of_header);
                total_content_bytes_received = total_bytes_received - actual_header_size;

                // content_bytes_received_in_current_part may be larger than content_chunk_size
                content_bytes_received_in_current_part = total_content_bytes_received;

                try
                {
                    incoming_content_length = packet.headers()[CONTENT_LENGTH].empty() ? 0 : std::stoi(
                            packet.headers()[CONTENT_LENGTH]);

                    if (incoming_content_length < 0)
                    {
                        error = true;
                        Log::error("HTTPProtocol", "{} is < 0: {}.", CONTENT_LENGTH, incoming_content_length);
                    }
                }
                catch (...)
                {
                    incoming_content_length = 0;
                }
            }
            else if (total_bytes_received >= max_header_size)
            {
                // Headers are too large
                response.reply_error(
                        std::make_unique<responses::ErrorResponse>(ResponseCode::Request_Header_Fields_Too_Large));

                Log::error("HTTPProtocol",
                           "Headers larger than max header size of {}: {} bytes.",
                           max_header_size,
                           total_bytes_received);
                reset();
            }
        }
        else
        {
            total_content_bytes_received += length;
            content_bytes_received_in_current_part += length;
        }

        if (is_complete(packet))
        {
            // Resize buffer to deliver exactly the number of received bytes to the application.
            using vector_type = std::remove_reference<decltype(packet.data())>::type;
            packet.data().resize(static_cast<vector_type::size_type>(content_bytes_received_in_current_part));

            packet.set_request_data(last_method, last_url, last_request_version);

            // When there are more data expected, then this packet is "to be continued"
            if (total_content_bytes_received < incoming_content_length)
            {
                packet.set_continued();
            }

            // When still reading the headers, the packet can never be a continuation.
            if (state != State::reading_headers)
            {
                // If we've already received more data than what fits in a single chunk, then this packet
                // is a continuation of an earlier packet.
                if (total_content_bytes_received - content_bytes_received_in_current_part > content_chunk_size)
                {
                    // Packet continues a previous packet.
                    packet.set_continuation();
                }
            }
        }
    }

    uint8_t* RegularHTTPProtocol::get_write_pos(HTTPPacket& packet)
    {
        int offset;

        if (state == State::reading_headers)
        {
            offset = total_bytes_received;
        }
        else
        {
            offset = content_bytes_received_in_current_part;
        }

        return &packet.data()[static_cast<std::vector<uint8_t>::size_type>(offset)];
    }

    bool RegularHTTPProtocol::is_complete(HTTPPacket& /*packet*/) const
    {
        auto complete = state != State::reading_headers;

        bool content_received =
            incoming_content_length == 0 // No content to read.
            || total_content_bytes_received == incoming_content_length // All content received
            || content_bytes_received_in_current_part >= content_chunk_size; // Packet filled, split into multiple
                                                                             // chunks.

        return complete && content_received;
    }

    bool RegularHTTPProtocol::is_error()
    {
        return error;
    }

    int RegularHTTPProtocol::consume_headers(HTTPPacket& packet,
                                             std::vector<uint8_t>::const_iterator header_ending)
    {
        std::stringstream ss;

        std::for_each(packet.data().cbegin(),
                      header_ending,
                      [&ss](auto& c) {
                          if (c != '\n')
                          {
                              ss << static_cast<char>(c);
                          }
        });

        // Get actual end of header
        header_ending += HTTPPacket::ending.size();

        // Update actual header size
        auto actual_header_bytes_received = static_cast<int>(std::distance(packet.data().cbegin(), header_ending));

        // Erase headers from buffer
        packet.data().erase(packet.data().begin(), header_ending);

        std::string s;

        while (std::getline(ss, s, '\r'))
        {
            if (!s.empty())
            {
                auto colon = std::find(s.begin(), s.end(), ':');

                if (colon == s.end() && !s.empty())
                {
                    std::smatch m;

                    if (std::regex_match(s, m, request_line))
                    {
                        // Store method for use in continued packets.
                        last_method = m[1].str();
                        last_url = m[2].str();
                        last_request_version = m[3].str();
                        packet.set_request_data(last_method, last_url, last_request_version);
                    }
                    else if (std::regex_match(s, m, response_line))
                    {
                        // Store response data for later use
                        try
                        {
                            auto response_code = std::stoi(m[2].str());
                            packet.set_response_data(static_cast<ResponseCode>(response_code));
                        }
                        catch (...)
                        {
                            error = true;

                            Log::error("HTTPProtocol", "Invalid response code: {}", m[2].str());
                        }
                    }
                }
                else
                {
                    if (std::distance(colon, s.end()) > 2)
                    {
                        // Headers are case-insensitive: https://tools.ietf.org/html/rfc7230#section-3.2
                        // Headers may be split on several lines, so append data if header isn't empty.
                        auto& curr_header = packet.headers()[string_util::to_lower_copy({ s.begin(), colon })];

                        if (curr_header.empty())
                        {
                            curr_header = { colon + 2, s.end() };
                        }
                        else
                        {
                            curr_header.append(", ").append({ colon + 2, s.end() });
                        }
                    }
                }
            }
        }

        return actual_header_bytes_received;
    }

    void RegularHTTPProtocol::packet_consumed()
    {
        content_bytes_received_in_current_part = 0;

        if (error || total_content_bytes_received >= incoming_content_length)
        {
            // All chunks of the current request has been received.
            total_bytes_received = 0;
            incoming_content_length = 0;
            total_content_bytes_received = 0;
            actual_header_size = 0;
            state = State::reading_headers;
        }

        error = false;
    }

    void RegularHTTPProtocol::reset()
    {
        // Simulate an error to force protocol to be completely reset.
        error = true;
        packet_consumed();
    }
}
