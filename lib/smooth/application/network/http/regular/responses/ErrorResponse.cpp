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

#include <smooth/application/network/http/regular/responses/ErrorResponse.h>

namespace smooth::application::network::http::regular::responses
{
    ErrorResponse::ErrorResponse(ResponseCode code)
            : smooth::application::network::http::regular::responses::StringResponse(
                  code, std::string{ std::to_string(static_cast<int>(code)) } + " " + response_code_to_text.at(code))
    {
    }
}
