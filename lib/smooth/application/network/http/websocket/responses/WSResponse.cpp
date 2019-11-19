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

#include "smooth/application/network/http/websocket/responses/WSResponse.h"
#include "smooth/core/network/util.h"

namespace smooth::application::network::http::websocket::responses
{
    ResponseStatus WSResponse::get_data(std::size_t max_amount, std::vector<uint8_t>& target)
    {
        // Portion data in such a way that at most max_amount of *data* is moved into target.

        auto res{ ResponseStatus::NoData };

        auto remaining = std::distance(data.begin(), data.end());

        if (!header_sent)
        {
            header_sent = true;
            uint8_t val;

            if (!first_fragment)
            {
                val = static_cast<uint8_t>(OpCode::Continuation);
            }
            else
            {
                val = static_cast<uint8_t>(op_code);
            }

            if (last_fragment)
            {
                // Set fin-bit
                val |= 0x80;
            }

            target.emplace_back(val);

            if (remaining == 0)
            {
                set_length(0, target);
                res = ResponseStatus::LastData;
            }
        }

        if (remaining > 0)
        {
            auto to_send = std::min(static_cast<std::size_t>(remaining), max_amount);

            set_length(to_send, target);

            auto begin = data.begin();
            auto end = data.begin() + static_cast<long>(to_send);

            std::copy(begin, end, std::back_inserter(target));
            data.erase(begin, end);

            // Anything still left?
            remaining = std::distance(data.begin(), data.end());
            res = remaining > 0 ? ResponseStatus::HasMoreData : ResponseStatus::LastData;
        }

        return res;
    }

    WSResponse::WSResponse(OpCode code)
            : op_code(code)
    {
    }

    WSResponse::WSResponse(const std::string& text, bool first_fragment, bool last_fragment)
            : op_code(OpCode::Text), first_fragment(first_fragment), last_fragment(last_fragment)
    {
        std::copy(text.begin(), text.end(), std::back_inserter(data));
    }

    WSResponse::WSResponse(std::string&& text, bool first_fragment, bool last_fragment)
            : op_code(OpCode::Text), first_fragment(first_fragment), last_fragment(last_fragment)
    {
        std::copy(std::make_move_iterator(text.begin()),
                  std::make_move_iterator(text.end()),
                  std::back_inserter(data));
    }

    WSResponse::WSResponse(const std::vector<uint8_t>& binary, bool treat_as_text, bool first_fragment,
                           bool last_fragment)
            : op_code(treat_as_text ? OpCode::Text : OpCode::Binary), first_fragment(first_fragment),
              last_fragment(last_fragment)
    {
        std::copy(binary.begin(), binary.end(), std::back_inserter(data));
    }

    WSResponse::WSResponse(std::vector<uint8_t>&& binary, bool first_fragment, bool last_fragment)
            : op_code(OpCode::Binary), first_fragment(first_fragment), last_fragment(last_fragment)
    {
        std::copy(std::make_move_iterator(binary.begin()),
                  std::make_move_iterator(binary.end()),
                  std::back_inserter(data));
    }

    void WSResponse::set_length(uint64_t len, std::vector<uint8_t>& buff) const
    {
        if (len <= 125)
        {
            buff.emplace_back(len);
        }
        else if (len > std::numeric_limits<uint16_t>::max())
        {
            auto size = smooth::core::network::hton(len);
            buff.emplace_back(127);
            auto p = reinterpret_cast<uint8_t*>(&size);
            std::copy(p, p + sizeof(size), std::back_inserter(buff));
        }
        else
        {
            auto l = static_cast<uint16_t>(len);
            std::uint16_t size = smooth::core::network::hton(l);
            buff.emplace_back(126);
            auto p = reinterpret_cast<uint8_t*>(&size);
            std::copy(p, p + sizeof(size), std::back_inserter(buff));
        }
    }
}
