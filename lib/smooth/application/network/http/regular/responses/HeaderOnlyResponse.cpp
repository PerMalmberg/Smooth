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

#include <utility>

#include <smooth/application/network/http/regular/responses/HeaderOnlyResponse.h>
#include <algorithm>
#include <smooth/core/logging/log.h>
#include <smooth/application/network/http/regular/HTTPHeaderDef.h>
#include <smooth/application/network/http/http_utils.h>
#include <smooth/core/util/string_util.h>

using namespace smooth::core::logging;
using namespace smooth::core;

namespace smooth::application::network::http::regular::responses
{
    HeaderOnlyResponse::HeaderOnlyResponse(ResponseCode code)
            : code(code)
    {
        headers[LAST_MODIFIED] = utils::make_http_time(std::chrono::system_clock::now());
    }

    ResponseCode HeaderOnlyResponse::get_response_code()
    {
        return code;
    }

    ResponseStatus HeaderOnlyResponse::get_data(std::size_t /*max_amount*/, std::vector<uint8_t>& /*target*/)
    {
        return ResponseStatus::NoData;
    }

    void HeaderOnlyResponse::set_header(const std::string& key, const std::string& value)
    {
        headers[key] = value;
    }

    void HeaderOnlyResponse::add_header(const std::string& key, const std::string& value)
    {
        auto& curr = headers[key];

        if (curr.empty())
        {
            set_header(key, value);
        }
        else if (!string_util::icontains(curr, value))
        {
            curr.append(", ").append(value);
        }
    }

    void HeaderOnlyResponse::dump() const
    {
        Log::debug("Response", Format("Code: {1}", Int32(static_cast<int>(code))));
    }
}
