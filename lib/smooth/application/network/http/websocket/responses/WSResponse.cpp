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