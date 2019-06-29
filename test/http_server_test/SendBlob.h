#pragma once

#include <smooth/application/network/http/responses/StringResponse.h>

namespace http_server_test
{
    class SendBlob
            : public smooth::application::network::http::responses::StringResponse
    {
        public:
            explicit SendBlob(std::size_t amount);

            ~SendBlob() override = default;

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            smooth::application::network::http::responses::ResponseStatus
            get_data(std::size_t max_amount, std::vector<uint8_t>& target) override;

        private:
            std::size_t amount_to_send;
    };
}