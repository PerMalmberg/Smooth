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

#include <string>
#include <chrono>
#include <smooth/core/filesystem/Path.h>
#include "HTTPMethod.h"

namespace smooth::application::network::http::utils
{
    std::string make_http_time(const std::chrono::system_clock::time_point& t);
    std::string make_http_time(const time_t& t);

    std::chrono::system_clock::time_point parse_http_time(const std::string& t);

    std::string get_content_type(const smooth::core::filesystem::Path& path);

    time_t timegm(tm& tm);

    std::string http_method_to_string(HTTPMethod m);
}