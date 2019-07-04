#include <smooth/application/network/http/HTTPProtocol.h>
#include <smooth/application/network/http/responses/ErrorResponse.h>
#include <algorithm>

namespace smooth::application::network::http
{

    int HTTPProtocol::get_wanted_amount(HTTPPacket& packet)
    {
        int amount_to_request;

        // Return the number of bytes wanted to fill the packet
        if (state == State::reading_headers)
        {
            // How much left until we reach max header size?
            amount_to_request = max_header_size - total_bytes_received;
            // Make sure there is room for what he have received and what we ask for.
            packet.ensure_room(total_bytes_received + amount_to_request);
        }
        else
        {
            // Never ask for more than content_chunk_size
            auto remaining_of_content = incoming_content_length - total_content_bytes_received;
            amount_to_request = std::min(content_chunk_size, remaining_of_content);

            packet.ensure_room(content_bytes_received_in_current_part + amount_to_request);
        }

        return amount_to_request;
    }


    void HTTPProtocol::data_received(HTTPPacket& packet, int length)
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
                        Log::error("HTTPProtocol",
                                   Format("{1} is < 0: {1}.", Str(CONTENT_LENGTH), Int32(incoming_content_length)));
                    }
                }
                catch (...)
                {
                    incoming_content_length = 0;
                }

                if (total_content_bytes_received < incoming_content_length)
                {
                    // There is more content to read, this packet will be followed by another packet.
                    packet.set_continued();
                }

                // When still reading the headers, the packet can never be a continuation, so don't check
                // for that condition.
            }
            else if (total_bytes_received >= max_header_size)
            {
                // Headers are too large
                response.reply_error(std::make_unique<responses::ErrorResponse>(ResponseCode::Request_Header_Fields_Too_Large));
                Log::error("HTTPProtocol",
                           Format("Headers larger than max header size of {1}: {2} bytes.", Int32(max_header_size), Int32(total_bytes_received)));
                reset();
            }
        }
        else
        {
            total_content_bytes_received += length;
            content_bytes_received_in_current_part += length;

            if (total_content_bytes_received < incoming_content_length)
            {
                // There is more content to read, this packet will be followed by another packet.
                packet.set_continued();
            }

            if (total_content_bytes_received > content_chunk_size)
            {
                // Packet continues a previous packet.
                packet.set_continuation();
            }
        }

        if (is_complete(packet))
        {
            // As headers are parsed and delivered separately from the content, we can resize the data buffer to
            // exactly fit the received content in this specific packet. This also makes it easy to later parse
            // the content since the exact size is known.  
            packet.data().resize(static_cast<std::vector<uint8_t>::size_type>(content_bytes_received_in_current_part));
                        
            packet.set_request_data(last_method, last_url, last_request_version);
        }
    }

    uint8_t* HTTPProtocol::get_write_pos(HTTPPacket& packet)
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


    bool HTTPProtocol::is_complete(HTTPPacket& /*packet*/) const
    {
        auto complete = state != State::reading_headers;

        bool content_received =
                // No content to read.
                incoming_content_length == 0
                // All content received
                || total_content_bytes_received == incoming_content_length
                // Packet filled, split into multiple chunks.
                || content_bytes_received_in_current_part >= content_chunk_size;

        return complete && content_received;
    }


    bool HTTPProtocol::is_error()
    {
        return error;
    }


    int HTTPProtocol::consume_headers(HTTPPacket& packet,
                                      std::vector<uint8_t>::const_iterator header_ending)
    {
        std::stringstream ss;

        std::for_each(packet.data().cbegin(), header_ending, [&ss](auto& c) {
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
                            Log::error("HTTPProtocol",
                                       Format("Invalid response code: {1}", Str(m[2].str())));
                        }
                    }
                }
                else
                {
                    if (std::distance(colon, s.end()) > 2)
                    {
                        // Headers are case-insensitive: https://tools.ietf.org/html/rfc7230#section-3.2
                        packet.headers()[util::to_lower_copy({s.begin(), colon})] = {colon + 2, s.end()};
                    }
                }
            }
        }

        return actual_header_bytes_received;
    }


    void HTTPProtocol::packet_consumed()
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


    void HTTPProtocol::reset()
    {
        // Simulate an error to force protocol to be completely reset.
        error = true;
        packet_consumed();
    }

}