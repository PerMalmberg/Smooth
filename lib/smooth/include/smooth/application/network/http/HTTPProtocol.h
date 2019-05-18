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
    template<int MaxHeaderSize, int ContentChuckSize>
    class HTTPProtocol
            : public smooth::core::network::IPacketAssembly<HTTPProtocol<MaxHeaderSize, ContentChuckSize>, HTTPPacket>
    {
            static_assert(MaxHeaderSize > 0,
                          "MaxHeaderSize must be larger than 0, and a reasonable value.");
            static_assert(ContentChuckSize > 0,
                          "ContentChuckSize must be larger than 0, and a reasonable value.");

        public:
            using packet_type = HTTPPacket;

            int get_wanted_amount(HTTPPacket& packet) override;

            void data_received(HTTPPacket& packet, int length) override;

            uint8_t* get_write_pos(HTTPPacket& packet) override;

            bool is_complete(HTTPPacket& packet) override;

            bool is_error() override;

            void packet_consumed() override;

            void reset() override;

        private:
            void parse_headers(HTTPPacket& packet);

            enum class State
            {
                    reading_headers,
                    reading_content
            };

            int incoming_content_length = 0;
            int header_bytes_received{0};
            int total_content_bytes_received = 0;
            int content_bytes_received_in_current_package{0};
            const std::regex request_line{R"!((.+)\ (.+)\ HTTP\/(\d\.\d))!"}; // "GET / HTTP/1.1"
            const char* CONTENT_LENGTH = "content-length";
            const char* tag = "HTTPProtocol";
            bool error = false;

            State state = State::reading_headers;
            std::string last_method{};
            std::string last_url{};
            std::string last_request_version{};
    };

    template<int MaxHeaderSize, int ContentChuckSize>
    int HTTPProtocol<MaxHeaderSize, ContentChuckSize>::get_wanted_amount(HTTPPacket& packet)
    {
        int res;

        // Return the number of bytes available in the buffer
        if (state == State::reading_headers)
        {
            if (header_bytes_received == static_cast<int>(packet.data().size()))
            {
                // Make room for one byte
                packet.increase_size(1);
            }

            res = 1;
        }
        else
        {
            // Don't make packets larger than ContentChuckSize
            res = std::min(incoming_content_length, ContentChuckSize - content_bytes_received_in_current_package);
            packet.set_size(res);
        }

        return res;
    }

    template<int MaxHeaderSize, int ContentChuckSize>
    void HTTPProtocol<MaxHeaderSize, ContentChuckSize>::data_received(HTTPPacket& packet, int length)
    {
        if (state == State::reading_headers)
        {
            header_bytes_received += length;

            if (packet.ends_with_two_crlf())
            {
                // End of header, parse it.
                parse_headers(packet);
                state = State::reading_content;

                try
                {
                    incoming_content_length = packet.headers()[CONTENT_LENGTH].empty() ? 0 : std::stoi(
                            packet.headers()[CONTENT_LENGTH]);
                    total_content_bytes_received = 0;
                }
                catch (...)
                {
                    incoming_content_length = 0;
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
        else
        {
            total_content_bytes_received += length;
            content_bytes_received_in_current_package += length;

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

            packet.set_request_data(last_method, last_url, last_request_version);
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
    void HTTPProtocol<MaxHeaderSize, ContentChuckSize>::parse_headers(HTTPPacket& packet)
    {
        std::stringstream ss;

        std::for_each(packet.data().cbegin(), packet.data().cend(), [&ss](auto& c) {
            if (c != '\n')
            {
                ss << static_cast<char>(c);
            }
        });

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

        packet.clear();
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


