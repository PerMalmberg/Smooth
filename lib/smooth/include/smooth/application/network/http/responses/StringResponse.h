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

#pragma once

#include "IRequestResponeOperation.h"
#include <vector>
#include <smooth/core/timer/ElapsedTime.h>

namespace smooth::application::network::http::responses
{
    class StringResponse
            : public IRequestResponseOperation
    {
        public:
            explicit StringResponse(ResponseCode code, std::string body = "");
            StringResponse& operator=(StringResponse&&) = default;
            StringResponse(StringResponse&&) = default;
            StringResponse& operator=(const StringResponse&) = delete;
            StringResponse(const StringResponse&) = delete;

            ~StringResponse() override = default;

            ResponseCode get_response_code() override;

            // Called once when beginning to send a response.
            const std::unordered_map<std::string, std::string>& get_headers() const override
            {
                return headers;
            };

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) override;

            void add_header(const std::string& key, const std::string& value) override;

            void dump() const override;

        protected:
            std::unordered_map<std::string, std::string> headers{};
            ResponseCode code;
        private:
            std::vector<uint8_t> data{};
            void add_string(std::string str);
    };
}