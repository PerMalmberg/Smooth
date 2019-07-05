#include <utility>


#include <smooth/application/network/http/responses/StringResponse.h>
#include <algorithm>
#include <smooth/core/logging/log.h>
#include <smooth/application/network/http/HTTPHeaderDef.h>
#include <smooth/application/network/http/http_utils.h>

namespace smooth::application::network::http::responses
{
    using namespace smooth::core::logging;

    StringResponse::StringResponse(ResponseCode code, std::string body)
            : code(code)
    {
        add_string("<html><body>");
        add_string(std::move(body));
        add_string("</body></html>");

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