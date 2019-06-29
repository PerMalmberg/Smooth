#pragma once

#include "IRequestResponeOperation.h"
#include <vector>
#include <smooth/core/timer/ElapsedTime.h>

namespace smooth::application::network::http::responses
{
    class StringResponse
            : public IRequestResponseOperation
    {
        public:
            explicit StringResponse(ResponseCode code, std::string body = "");
            StringResponse& operator=(StringResponse&&) = default;
            StringResponse(StringResponse&&) = default;
            StringResponse& operator=(const StringResponse&) = delete;
            StringResponse(const StringResponse&) = delete;

            ~StringResponse() override = default;

            ResponseCode get_response_code() override;

            // Called once when beginning to send a response.
            const std::unordered_map<std::string, std::string>& get_headers() const override
            {
                return headers;
            };

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) override;

            void add_header(const std::string& key, const std::string& value) override;

            void dump() const override;

        protected:
            std::unordered_map<std::string, std::string> headers{};
            ResponseCode code;
        private:
            std::vector<uint8_t> data{};
            void add_string(std::string str);
    };
}