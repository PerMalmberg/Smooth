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

#include <unordered_map>
#include <smooth/core/network/BufferContainer.h>
#include <smooth/application/network/http/regular/ResponseCodes.h>

namespace smooth::application::network::http
{
    enum class ResponseStatus
    {
            Error,
            HasMoreData,
            LastData,
            NoData
    };

    // A request operation is responsible for providing outgoing data chunked into smaller pieces
    // as per the passed arguments.
    class IResponseOperation
    {
        public:
            virtual ~IResponseOperation() = default;

            virtual regular::ResponseCode get_response_code()
            {
                return regular::ResponseCode::Not_Found;
            }

            const std::unordered_map<std::string, std::string>& get_headers() const
            {
                return headers;
            }

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            virtual ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) = 0;

            /// Sets a header, replacing any existing value
            virtual void set_header(const std::string& /*key*/, const std::string& /*value*/)
            {}

            /// Adds a header value, appending to any existing value.
            virtual void add_header(const std::string& /*key*/, const std::string& /*value*/)
            {};

            virtual void dump() const
            {}

        protected:
            std::unordered_map<std::string, std::string> headers{};
    };
}