#pragma once

#include <string>
#include <chrono>
#include <smooth/core/filesystem/Path.h>

namespace smooth::application::network::http::utils
{
    std::string format_last_modified(const time_t& t);
    std::chrono::system_clock::time_point parse_http_time(const std::string& t);
    std::string get_content_type(const smooth::core::filesystem::Path& path);
}