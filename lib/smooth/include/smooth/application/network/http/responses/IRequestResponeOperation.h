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
#include <smooth/application/network/http/ResponseCodes.h>

namespace smooth::application::network::http::responses
{
    enum class ResponseStatus
    {
            Error,
            HasMoreData,
            LastData,
            EndOfData
    };

    // A request operation is responsible for providing outgoing data chunked into smaller pieces
    // as per the passed arguments.
    class IRequestResponseOperation
    {
        public:
            virtual ~IRequestResponseOperation() = default;

            virtual ResponseCode get_response_code() = 0;

            // Called once when beginning to send a response.
            virtual const std::unordered_map<std::string, std::string>& get_headers() const = 0;

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            virtual ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) = 0;

            /// Adds a header to the reply.
            virtual void add_header(const std::string& key, const std::string& value) = 0;

            virtual void dump() const = 0;
    };
}