//
// Created by permal on 2019-05-09.
//

#include "SendBlob.h"
#include <smooth/application/network/http/ResponseCodes.h>

using namespace smooth::application::network::http;
using namespace smooth::application::network::http::responses;

namespace http_server_test
{
    http_server_test::SendBlob::SendBlob(std::size_t amount)
            : Response(ResponseCode::OK), amount_to_send(amount)
    {
        headers["content-length"] = std::to_string(amount_to_send);
        headers["content-type"] = "application/octet-stream";
    }

    ResponseStatus
    http_server_test::SendBlob::get_data(std::size_t max_amount, std::vector<uint8_t>& target)
    {
        auto res = ResponseStatus::AllSent;

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