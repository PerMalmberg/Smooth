#pragma once

#include "IRequestResponeOperation.h"

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                namespace responses
                {
                    class NotFound_404 : public IRequestResponseOperation
                    {
                        public:
                            ResponseCode get_response_code() override;

                            // Called once when beginning to send a response.
                            void get_headers(std::unordered_map<std::string, std::string>& headers) override;

                            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
                            ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) override;
                    };
                }
            }
        }
    }
}