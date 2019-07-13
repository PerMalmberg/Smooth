// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <unordered_map>
#include <memory>
#include "RequestHandlerSignature.h"
#include "IServerResponse.h"

namespace smooth::application::network::http::regular
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