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

#include "SendBlob.h"
#include "smooth/application/network/http/regular/ResponseCodes.h"
#include "smooth/application/network/http/regular/HTTPHeaderDef.h"

using namespace smooth::application::network::http;
using namespace smooth::application::network::http::regular;
using namespace smooth::application::network::http::regular::responses;

namespace http_server_test
{
    http_server_test::SendBlob::SendBlob(std::size_t amount)
            : StringResponse(ResponseCode::OK), amount_to_send(amount)
    {
        headers[CONTENT_LENGTH] = std::to_string(amount_to_send);
        headers[CONTENT_TYPE] = "application/octet-stream";
    }

    ResponseStatus http_server_test::SendBlob::get_data(std::size_t max_amount, std::vector<uint8_t>& target)
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
