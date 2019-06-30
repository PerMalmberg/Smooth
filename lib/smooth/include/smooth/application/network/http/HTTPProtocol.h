#pragma once

#include <cstdint>
#include <regex>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/IPacketDisassembly.h>
#include <smooth/core/network/IPacketAssembly.h>
#include <smooth/core/util/string_util.h>
#include "HTTPPacket.h"
#include "HTTPHeaderDef.h"
#include "IServerResponse.h"

using namespace smooth::core;
using namespace smooth::core::logging;

namespace smooth::application::network::http
{

    class HTTPProtocol
            : public smooth::core::network::IPacketAssembly<HTTPProtocol, HTTPPacket>
    {
            /*qqq static_assert(MaxHeaderSize > 0,
                          "MaxHeaderSize must be larger than 0, and a reasonable value.");
            static_assert(ContentChunkSize > 0,
                          "ContentChunkSize must be larger than 0, and a reasonable value.");
            static_assert(ContentChunkSize >= MaxHeaderSize,
                          "ContentChunkSize must be  larger or equal to MaxHeaderSize"); */

        public:
            using packet_type = HTTPPacket;

            HTTPProtocol(int max_header_size, int content_chunk_size, IServerResponse& response)
                    : max_header_size(max_header_size),
                      content_chunk_size(content_chunk_size),
                      response(response)
            {
            }

            int get_wanted_amount(HTTPPacket& packet) override;

            void data_received(HTTPPacket& packet, int length) override;

            uint8_t* get_write_pos(HTTPPacket& packet) override;

            bool is_complete(HTTPPacket& packet) const override;

            bool is_error() override;

            void packet_consumed() override;

            void reset() override;

        private:
            int consume_headers(HTTPPacket& packet, std::vector<uint8_t>::const_iterator header_ending);

            enum class State
            {
                    reading_headers,
                    reading_content
            };

            const int max_header_size;
            const int content_chunk_size;
            IServerResponse& response;

            int total_bytes_received{0};
            int total_content_bytes_received{0};
            int content_bytes_received_in_current_part{0};
            int incoming_content_length{0};
            int actual_header_size{0};

            const std::regex response_line{R"!(HTTP\/(\d.\d)\ (\d+)\ (.+))!"}; // HTTP/1.1 200 OK
            const std::regex request_line{R"!((.+)\ (.+)\ HTTP\/(\d\.\d))!"}; // "GET / HTTP/1.1"

            bool error = false;
            State state = State::reading_headers;
            std::string last_method{};
            std::string last_url{};

            std::string last_request_version{};
    };
}


