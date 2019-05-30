#pragma once

#include <unordered_map>
#include <smooth/core/network/BufferContainer.h>
#include <smooth/application/network/http/ResponseCodes.h>

namespace smooth::application::network::http::responses
{
    enum class ResponseStatus
    {
            Error,
            HasMoreData,
            AllSent
    };

    // A request operation is responsible for providing outgoing data chunked into smaller pieces
    // as per the passed arguments.
    class IRequestResponseOperation
    {
        public:
            virtual ~IRequestResponseOperation() = default;

            virtual ResponseCode get_response_code() = 0;

            // Called once when beginning to send a response.
            virtual const std::unordered_map<std::string, std::string>& get_headers() const = 0;

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            virtual ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) = 0;

            /// Adds a header to the reply.
            virtual void add_header(const std::string& key, const std::string& value) = 0;

            virtual void dump() const = 0;
    };
}