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


#include <smooth/application/network/http/responses/StringResponse.h>
#include <algorithm>
#include <smooth/core/logging/log.h>
#include <smooth/application/network/http/HTTPHeaderDef.h>
#include <smooth/application/network/http/http_utils.h>

using namespace smooth::core::logging;

namespace smooth::application::network::http::responses
{
    StringResponse::StringResponse(ResponseCode code, std::string body, bool add_surrounding_html)
            : code(code)
    {
        if(add_surrounding_html)
        {
            add_string("<html><body>");
        }

        add_string(std::move(body));

        if(add_surrounding_html)
        {
            add_string("</body></html>");
        }

        headers[CONTENT_LENGTH] = std::to_string(data.size());
        headers[CONTENT_TYPE] = "text/html";
        headers[LAST_MODIFIED] = utils::make_http_time(std::chrono::system_clock::now());
    }

    ResponseCode StringResponse::get_response_code()
    {
        return code;
    }

    ResponseStatus StringResponse::get_data(std::size_t max_amount, std::vector<uint8_t>& target)
    {
        auto res{ResponseStatus::EndOfData};

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

    void StringResponse::add_header(const std::string& key, const std::string& value)
    {
        headers[key] = value;
    }

    void StringResponse::dump() const
    {
        Log::debug("Response", Format("Code: {1}; Remaining: {2} bytes", Int32(static_cast<int>(code)), UInt64(data.size())));
    }

    void StringResponse::add_string(std::string str)
    {
        data.insert(data.end(), std::make_move_iterator(str.begin()),
                    std::make_move_iterator(str.end()));
    }
}