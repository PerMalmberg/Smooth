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

#pragma once

#include <string>
#include <chrono>
#include "smooth/core/filesystem/Path.h"
#include "regular/HTTPMethod.h"

namespace smooth::application::network::http::utils
{
    std::string make_http_time(const std::chrono::system_clock::time_point& t);

    std::string make_http_time(const time_t& t);

    std::chrono::system_clock::time_point parse_http_time(const std::string& t);

    std::string get_content_type(const smooth::core::filesystem::Path& path);

    time_t timegm(tm& tm);

    std::string http_method_to_string(regular::HTTPMethod m);
}
