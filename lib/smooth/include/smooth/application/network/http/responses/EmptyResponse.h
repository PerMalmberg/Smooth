#pragma once

#include "Response.h"

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
                    class EmptyResponse
                            : public Response
                    {
                        public:
                            explicit EmptyResponse(ResponseCode code);

                            // Called once when beginning to send a response.
                            void get_headers(std::unordered_map<std::string, std::string>& headers) override;
                    };
                }
            }
        }
    }
}