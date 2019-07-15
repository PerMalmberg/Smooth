#include <smooth/application/network/http/websocket/responses/WSResponse.h>
#include <smooth/core/network/util.h>

namespace smooth::application::network::http::websocket::responses
{
    ResponseStatus WSResponse::get_data(std::size_t max_amount, std::vector<uint8_t>& target)
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

    WSResponse::WSResponse(WebsocketProtocol::OpCode code)
    {
        // Set fin-bit
        auto val = 0x80 | static_cast<uint8_t>(code);
        data.emplace_back(val);
        // No payload.
        data.emplace_back(0);
    }

    WSResponse::WSResponse(const std::string& text)
    {
        auto val = 0x80 | static_cast<uint8_t>(WebsocketProtocol::OpCode::Text);
        data.emplace_back(val);
        if(text.size() <= 125)
        {
            data.emplace_back(text.size());
        }
        else if(text.size() > std::numeric_limits<uint16_t>::max())
        {
            std::array<uint8_t, 8> size{};
            auto p = reinterpret_cast<uint64_t*>(&size[0]);
            *p = static_cast<uint64_t>(text.size());
            *p = smooth::core::network::hton(*p);
            data.emplace_back(127);
            std::copy(size.begin(), size.end(), std::back_inserter(data));
            data.emplace_back(text.size());
        }
        else
        {
            std::array<uint8_t, 2> size{};
            auto p = reinterpret_cast<uint16_t*>(&size[0]);
            *p = static_cast<uint16_t>(text.size());
            *p = smooth::core::network::hton(*p);
            data.emplace_back(126);
            std::copy(size.begin(), size.end(), std::back_inserter(data));
            data.emplace_back(text.size());
        }


        std::copy(text.begin(), text.end(), std::back_inserter(data));
    }
}