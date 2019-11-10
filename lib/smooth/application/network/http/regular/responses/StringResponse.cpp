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

#include "smooth/application/network/http/regular/responses/StringResponse.h"
#include <algorithm>
#include "smooth/core/logging/log.h"
#include "smooth/application/network/http/regular/HTTPHeaderDef.h"
#include "smooth/application/network/http/http_utils.h"

using namespace smooth::core::logging;

namespace smooth::application::network::http::regular::responses
{
    StringResponse::StringResponse(ResponseCode code, std::string body, bool add_surrounding_html)
            : HeaderOnlyResponse(code)
    {
        if (add_surrounding_html)
        {
            add_string("<html><body>");
        }

        add_string(std::move(body));

        if (add_surrounding_html)
        {
            add_string("</body></html>");
        }

        headers[CONTENT_LENGTH] = std::to_string(data.size());
        headers[CONTENT_TYPE] = "text/html";
        headers[LAST_MODIFIED] = utils::make_http_time(std::chrono::system_clock::now());
    }

    ResponseStatus StringResponse::get_data(std::size_t max_amount, std::vector<uint8_t>& target)
    {
        auto res{ ResponseStatus::NoData };

        auto remaining = std::distance(data.begin(), data.end());

        if (remaining > 0)
        {
            auto to_send = std::min(static_cast<std::size_t>(remaining), max_amount);
            auto begin = data.begin();
            auto end = data.begin() + static_cast<long>(to_send);

            std::copy(std::make_move_iterator(begin), std::make_move_iterator(end),
                      std::back_inserter(target));
            data.erase(begin, end);

            // Anything still left?
            remaining = std::distance(data.begin(), data.end());
            res = remaining > 0 ? ResponseStatus::HasMoreData : ResponseStatus::LastData;
        }

        return res;
    }

    void StringResponse::dump() const
    {
        Log::debug("Response", "Code: {}; Remaining: {} bytes", code, data.size());
    }

    void StringResponse::add_string(std::string str)
    {
        data.insert(data.end(), std::make_move_iterator(str.begin()),
                    std::make_move_iterator(str.end()));
    }
}
