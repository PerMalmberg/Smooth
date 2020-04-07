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

#include "SendBlob.h"
#include <smooth/application/network/http/regular/ResponseCodes.h>
#include <smooth/application/network/http/regular/HTTPHeaderDef.h>

using namespace smooth::application::network::http;
using namespace smooth::application::network::http::regular;
using namespace smooth::application::network::http::regular::responses;

namespace http_server_test2
{
    http_server_test2::SendBlob::SendBlob(std::size_t amount)
            : StringResponse(ResponseCode::OK), amount_to_send(amount)
    {
        headers[CONTENT_LENGTH] = std::to_string(amount_to_send);
        headers[CONTENT_TYPE] = "application/octet-stream";
    }

    ResponseStatus
    http_server_test2::SendBlob::get_data(std::size_t max_amount, std::vector<uint8_t>& target)
    {
        auto res = ResponseStatus::NoData;

        if (amount_to_send > 0)
        {
            auto size = std::min(max_amount, amount_to_send);

            for (decltype(size) i = 0; i < size; ++i)
            {
                target.push_back('x');
            }

            amount_to_send -= size;

            res = ResponseStatus::HasMoreData;
        }

        return res;
    }
}