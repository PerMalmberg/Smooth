#pragma once

#include <unordered_map>
#include "ResponseSignature.h"

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                class IRequestHandler
                {
                    public:
                        virtual ~IRequestHandler() = default;

                        virtual void handle_post(const std::string& requested_url,
                                                 const std::unordered_map<std::string, std::string>& request_headers,
                                                 const std::unordered_map<std::string, std::string>& request_parameters,
                                                 const std::vector<uint8_t>& data,
                                                 bool fist_part,
                                                 bool last_part) = 0;
                };
            }
        }
    }
}