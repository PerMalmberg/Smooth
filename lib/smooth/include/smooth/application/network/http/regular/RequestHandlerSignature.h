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

#include <memory>
#include "smooth/application/network/http/IResponseOperation.h"
#include <smooth/application/network/http/IConnectionTimeoutModifier.h>
#include "smooth/application/network/http/IServerResponse.h"

namespace smooth::application::network::http::regular
{
    class MIMEParser;

    using RequestHandlerSignature = std::function<void(
            IServerResponse& response,
            IConnectionTimeoutModifier& timeout_modifier,
            const std::string& url,
            bool first_part,
            bool last_part,
            const std::unordered_map<std::string, std::string>& headers,
            const std::unordered_map<std::string, std::string>& request_parameters,
            const std::vector<uint8_t>& content,
            MIMEParser& mime
    )>;

}