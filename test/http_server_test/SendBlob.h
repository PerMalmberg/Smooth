#pragma once

#include <smooth/application/network/http/responses/Response.h>

namespace http_server_test
{
    class SendBlob
            : public smooth::application::network::http::responses::Response
    {
        public:
            explicit SendBlob(std::size_t amount);

            ~SendBlob() override = default;

            // Called once when beginning to send a response.
            void get_headers(std::unordered_map<std::string, std::string>& headers) override;

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            smooth::application::network::http::responses::ResponseStatus
            get_data(std::size_t max_amount, std::vector<uint8_t>& target) override;

        private:
            std::size_t amount_to_send;
    };
}