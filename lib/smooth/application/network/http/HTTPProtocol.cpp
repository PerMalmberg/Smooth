#include <smooth/application/network/http/HTTPProtocol.h>

namespace smooth::application::network::http
{

    int HTTPProtocol::get_wanted_amount(HTTPPacket& packet)
    {
        auto& data = packet.data();

        // If there is data remaining from the previous read, move that into the current packet
        if (!remains.empty())
        {
            content_bytes_received = static_cast<int>(remains.size());
            std::move(remains.begin(), remains.end(), std::back_inserter(data));
            remains.clear();
        }

        int res;

        // Return the number of bytes wanted to fill the packet
        if (state == State::reading_headers)
        {
            // How much left until we reach max header size?
            res = max_header_size - total_bytes_received;
            // Make sure there is room for what he have received and what we ask for.
            packet.ensure_room(total_bytes_received + res);
        }
        else
        {
            // QQQ Ignore content_chunk_size for now
            auto remaining_of_content = incoming_content_length - content_bytes_received;
            packet.ensure_room(total_bytes_received + remaining_of_content);

            res = remaining_of_content;

        }

        return res;
    }


    void HTTPProtocol::data_received(HTTPPacket& packet, int length)
    {
        total_bytes_received += length;

        if(state == State::reading_headers)
        {
            const auto end_of_header = packet.find_header_ending();

            if (end_of_header != packet.data().end())
            {
                // End of header found
                state = State::reading_content;
                actual_header_size = consume_headers(packet, end_of_header);
                content_bytes_received = total_bytes_received - actual_header_size;

                try
                {
                    incoming_content_length = packet.headers()[CONTENT_LENGTH].empty() ? 0 : std::stoi(
                            packet.headers()[CONTENT_LENGTH]);
                }
                catch (...)
                {
                    incoming_content_length = 0;
                }
            }
            else if(total_bytes_received >= max_header_size)
            {
                // Headers are too large
                error = true;
                Log::error("HTTPProtocol",
                        Format("Headers larger than MaxHeaderSize of {1} bytes.", Int32(max_header_size)));
            }
        }

        if(!error && state != State::reading_headers)
        {
            content_bytes_received = total_bytes_received - actual_header_size;

            // Headers are parsed so we're now dealing with the body content.
            if(content_bytes_received == incoming_content_length)
            {
                // All content received
            }
        }

        set_continuation_indicators(packet);
        packet.set_request_data(last_method, last_url, last_request_version);
    }


    void HTTPProtocol::set_continuation_indicators(HTTPPacket& /*packet*/) const
    {
        /*if (content_length_received > content_chunk_size)
        {
            // Packet continues a previous packet.
            packet.set_continuation();
        }*/

        /*if (content_length_received < incoming_content_length)
        {
            // There is more content to read, this packet will be followed by another packet.
            packet.set_continued();
        }*/
    }


    uint8_t* HTTPProtocol::get_write_pos(HTTPPacket& packet)
    {
        return &packet.data()[static_cast<std::vector<uint8_t>::size_type>(total_bytes_received)];
    }


    bool HTTPProtocol::is_complete(HTTPPacket& /*packet*/)
    {
        auto complete = state != State::reading_headers;

        bool content_received = incoming_content_length == 0 // No content to read.
                                 || content_bytes_received ==
                                    incoming_content_length; // All content received
                                 /*|| content_bytes_received_in_current_package ==
                                    content_chunk_size; // Packet filled, split into multiple.*/

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
        // Erase header
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
                        Log::info("QQQ ===> ", last_url);
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
        total_bytes_received = 0;

        if (error || content_bytes_received == incoming_content_length)
        {
            // All chunks of the current request has been received.
            incoming_content_length = 0;
            content_bytes_received = 0;
            remains.clear();
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