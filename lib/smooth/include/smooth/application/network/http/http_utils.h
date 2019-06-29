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