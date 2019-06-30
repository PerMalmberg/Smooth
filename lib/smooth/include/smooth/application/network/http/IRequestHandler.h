#pragma once

#include <unordered_map>
#include <memory>
#include "RequestHandlerSignature.h"
#include "IServerResponse.h"

namespace smooth::application::network::http
{
    class MIMEParser;

    class IRequestHandler
    {
        public:
            virtual ~IRequestHandler() = default;

            virtual void handle(HTTPMethod method,
                                IServerResponse& response,
                                const std::string& requested_url,
                                const std::unordered_map<std::string, std::string>& request_headers,
                                const std::unordered_map<std::string, std::string>& request_parameters,
                                const std::vector<uint8_t>& data,
                                MIMEParser& mime,
                                bool fist_part,
                                bool last_part) = 0;
    };
}