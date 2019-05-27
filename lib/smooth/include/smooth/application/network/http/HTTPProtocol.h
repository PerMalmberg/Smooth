#pragma once

#include <cstdint>
#include <regex>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/IPacketDisassembly.h>
#include <smooth/core/network/IPacketAssembly.h>
#include <smooth/core/util/string_util.h>
#include "HTTPPacket.h"

using namespace smooth::core;
using namespace smooth::core::logging;

namespace smooth::application::network::http
{
    template<int MaxHeaderSize, int ContentChunkSize>
    class HTTPProtocol
            : public smooth::core::network::IPacketAssembly<HTTPProtocol<MaxHeaderSize, ContentChunkSize>, HTTPPacket>
    {
            static_assert(MaxHeaderSize > 0,
                          "MaxHeaderSize must be larger than 0, and a reasonable value.");
            static_assert(ContentChunkSize > 0,
                          "ContentChunkSize must be larger than 0, and a reasonable value.");
            static_assert(ContentChunkSize >= MaxHeaderSize,
                          "ContentChunkSize must be  larger or equal to MaxHeaderSize");

        public:
            using packet_type = HTTPPacket;

            HTTPProtocol()
            {
                remains.reserve(MaxHeaderSize);
            }

            int get_wanted_amount(HTTPPacket& packet) override;

            void data_received(HTTPPacket& packet, int length) override;

            uint8_t* get_write_pos(HTTPPacket& packet) override;

            bool is_complete(HTTPPacket& packet) override;

            bool is_error() override;

            void packet_consumed() override;

            void reset() override;

        private:
            int consume_headers(HTTPPacket& packet, std::vector<uint8_t>::const_iterator header_ending);

            void set_continuation_indicators(HTTPPacket& packet) const;

            enum class State
            {
                    reading_headers,
                    reading_content
            };
            int incoming_content_length = 0;
            int header_bytes_received{0};
            int total_content_bytes_received = 0;
            int content_bytes_received_in_current_package{0};
            const std::regex response_line{R"!(HTTP\/(\d.\d)\ (\d+)\ (.+))!"}; // HTTP/1.1 200 OK
            const std::regex request_line{R"!((.+)\ (.+)\ HTTP\/(\d\.\d))!"}; // "GET / HTTP/1.1"
            const char* CONTENT_LENGTH = "content-length";
            const char* tag = "HTTPProtocol";

            bool error = false;
            State state = State::reading_headers;
            std::string last_method{};
            std::string last_url{};

            std::string last_request_version{};

            std::vector<uint8_t> remains{};
    };

    template<int MaxHeaderSize, int ContentChuckSize>
    int HTTPProtocol<MaxHeaderSize, ContentChuckSize>::get_wanted_amount(HTTPPacket& packet)
    {
        auto& data = packet.data();

        // If there is data remaining from the previous read, move that into the current packet
        if(!remains.empty())
        {
            content_bytes_received_in_current_package = static_cast<int>(remains.size());
            std::move(remains.begin(), remains.end(), std::back_inserter(data));
            remains.clear();
        }

        int res;

        // Return the number of bytes wanted to fill the packet
        if (state == State::reading_headers)
        {
            res = MaxHeaderSize - header_bytes_received;
            packet.ensure_room(header_bytes_received + res);
        }
        else
        {
            // Don't make packets larger than ContentChunkSize
            res = std::min(incoming_content_length - total_content_bytes_received, ContentChuckSize - content_bytes_received_in_current_package);
            packet.ensure_room(content_bytes_received_in_current_package + res);
        }

        return res;
    }

    template<int MaxHeaderSize, int ContentChuckSize>
    void HTTPProtocol<MaxHeaderSize, ContentChuckSize>::data_received(HTTPPacket& packet, int length)
    {
        if (state == State::reading_headers)
        {
            header_bytes_received += length;

            const auto end_of_header = packet.get_header_ending();
            if (end_of_header != packet.data().end())
            {
                state = State::reading_content;

                auto total_bytes_so_far = header_bytes_received;

                // End of header detected, consume it from the packet.
                header_bytes_received = consume_headers(packet, end_of_header);

                content_bytes_received_in_current_package = total_bytes_so_far - header_bytes_received;
                total_content_bytes_received = content_bytes_received_in_current_package;

                try
                {
                    incoming_content_length = packet.headers()[CONTENT_LENGTH].empty() ? 0 : std::stoi(
                            packet.headers()[CONTENT_LENGTH]);
                }
                catch (...)
                {
                    incoming_content_length = 0;
                }

                auto& data = packet.data();

                // Any data not part of the headers must be considered part of the content, or part of the next packet.
                if (content_bytes_received_in_current_package >= incoming_content_length)
                {
                    // The packet is so small we received it all while reading the header, and we possibly
                    // also have read part of the next packet.

                    // Do we have data for next packet?
                    if(content_bytes_received_in_current_package > incoming_content_length)
                    {
                        // We do, move parts after current content to other buffer.

                        // Test command to trigger this branch:
                        // echo "GET / HTTP/1.1\r\n\r\nGET / HTTP/1.1\r\n\Connection: keep-alive\r\nContent-Length: 5\r\n12345\r\n\r\n" |nc localhost 8080
                        // Note that if this path is triggered, another readable() event is required for the stored data
                        // to actually be used, there is no recursive calls to force it to be processed.

                        remains.clear();
                        auto length_of_extra_data = content_bytes_received_in_current_package - incoming_content_length;
                        auto start_next = data.begin() + incoming_content_length;
                        auto end_next = start_next + length_of_extra_data;
                        std::move(start_next, end_next, std::back_inserter(remains));
                        data.erase(std::remove_if(start_next, data.end(), [](auto&){return true;}), data.end());
                    }

                    // Must trim the packet since we reserved MaxHeaderSize in get_wanted_amount() to be able to do
                    // block reads but we don't want to count the extra space after the actual data.
                    if(std::distance(data.begin(), data.end()) > content_bytes_received_in_current_package)
                    {
                        data.erase(std::remove_if(data.begin() + content_bytes_received_in_current_package, data.end(),
                                                  [](auto&) { return true; }), data.end());
                    }

                    // Only data for current packet left at this point
                    const auto current_size = static_cast<int>(data.size());
                    total_content_bytes_received = current_size;
                    content_bytes_received_in_current_package = current_size;

                    set_continuation_indicators(packet);
                }
            }
            else if (header_bytes_received > MaxHeaderSize)
            {
                // Headers are too large
                error = true;
                Log::error("HTTPProtocol",
                           Format("Headers larger than MaxHeaderSize of {1} bytes.", Int32(MaxHeaderSize)));
            }
        }
        else // reading_content
        {
            total_content_bytes_received += length;
            content_bytes_received_in_current_package += length;

            set_continuation_indicators(packet);

            packet.set_request_data(last_method, last_url, last_request_version);
        }
    }

    template<int MaxHeaderSize, int ContentChuckSize>
    void HTTPProtocol<MaxHeaderSize, ContentChuckSize>::set_continuation_indicators(HTTPPacket& packet) const
    {
        if (total_content_bytes_received > ContentChuckSize)
        {
            // Packet continues a previous packet.
            packet.set_continuation();
        }

        if (total_content_bytes_received < incoming_content_length)
        {
            // There is more content to read, this packet will be followed by another packet.
            packet.set_continued();
        }
    }

    template<int MaxHeaderSize, int ContentChuckSize>
    uint8_t* HTTPProtocol<MaxHeaderSize, ContentChuckSize>::get_write_pos(HTTPPacket& packet)
    {
        auto offset = state == State::reading_content ? content_bytes_received_in_current_package
                                                      : header_bytes_received;

        return &packet.data()[static_cast<std::vector<uint8_t>::size_type>(offset)];
    }

    template<int MaxHeaderSize, int ContentChuckSize>
    bool HTTPProtocol<MaxHeaderSize, ContentChuckSize>::is_complete(HTTPPacket& /*packet*/)
    {
        auto complete = state != State::reading_headers;

        bool content_received = (incoming_content_length == 0 // No content to read.
                                 || total_content_bytes_received ==
                                    incoming_content_length // All content received
                                 || content_bytes_received_in_current_package ==
                                    ContentChuckSize); // Packet filled, split into multiple.

        return complete && content_received;
    }

    template<int MaxHeaderSize, int ContentChuckSize>
    bool HTTPProtocol<MaxHeaderSize, ContentChuckSize>::is_error()
    {
        return error;
    }

    template<int MaxHeaderSize, int ContentChuckSize>
    int HTTPProtocol<MaxHeaderSize, ContentChuckSize>::consume_headers(HTTPPacket& packet,
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


//        Log::debug(tag, Format("{1} {2} HTTP/{3}", Str(last_method), Str(last_url), Str(last_request_version)));
//        for (const auto& header : packet.headers())
//        {
//            Log::debug(tag, Format("{1}: {2}", Str(header.first), Str(header.second)));
//        }

        return actual_header_bytes_received;
    }

    template<int MaxHeaderSize, int ContentChuckSize>
    void HTTPProtocol<MaxHeaderSize, ContentChuckSize>::packet_consumed()
    {
        content_bytes_received_in_current_package = 0;

        if (error || total_content_bytes_received == incoming_content_length)
        {
            // All chunks of the current request has been received.
            incoming_content_length = 0;
            header_bytes_received = 0;
            total_content_bytes_received = 0;
            state = State::reading_headers;
        }

        error = false;
    }

    template<int MaxHeaderSize, int ContentChuckSize>
    void HTTPProtocol<MaxHeaderSize, ContentChuckSize>::reset()
    {
        // Simulate an error to force protocol to be completely reset.
        error = true;
        packet_consumed();
    }
}


