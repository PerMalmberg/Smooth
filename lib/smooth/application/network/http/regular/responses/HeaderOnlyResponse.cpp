/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <utility>

#include "smooth/application/network/http/regular/responses/HeaderOnlyResponse.h"
#include <algorithm>
#include "smooth/core/logging/log.h"
#include "smooth/application/network/http/regular/HTTPHeaderDef.h"
#include "smooth/application/network/http/http_utils.h"
#include "smooth/core/util/string_util.h"

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
        Log::debug("Response", "Code: {}", code);
    }
}
