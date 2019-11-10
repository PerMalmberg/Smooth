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

#include "smooth/application/network/http/http_utils.h"

#include <array>
#include <sstream>
#include <iomanip>
#include <mutex>

using namespace smooth::application::network::http::regular;
using namespace std::chrono;

namespace smooth::application::network::http::utils
{
    static const std::array<const char*, 7> day{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    static const std::array<const char*, 12> month{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep",
                                                    "Oct", "Nov", "Dec" };

    std::string make_http_time(const std::chrono::system_clock::time_point& t)
    {
        auto tt = std::chrono::system_clock::to_time_t(t);

        return make_http_time(tt);
    }

    std::string make_http_time(const time_t& t)
    {
        tm time{};
        gmtime_r(&t, &time);
        std::stringstream ss;

        // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Last-Modified
        // GMT == UTC and the time read from disc is in UTC so no need to convert between timezones.
        // <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
        ss << day[static_cast<decltype(day)::size_type>(time.tm_wday)];
        ss << ", " << std::setfill('0') << std::setw(2) << time.tm_mday;
        ss << " " << month[static_cast<decltype(month)::size_type>(time.tm_mon)];
        ss << " " << time.tm_year + 1900;
        ss << " " << std::setfill('0') << std::setw(2) << time.tm_hour;
        ss << ":" << std::setfill('0') << std::setw(2) << time.tm_min;
        ss << ":" << std::setfill('0') << std::setw(2) << time.tm_sec;
        ss << " GMT";

        return ss.str();
    }

    std::chrono::system_clock::time_point parse_http_time(const std::string& t)
    {
        tm time{};

        // Default locale is "C"
        std::istringstream ss(t);

        system_clock::time_point res{};

        ss >> std::get_time(&time, "%a, %d %b %Y %H:%M:%S GMT");

        if (ss.fail())
        {
            res = system_clock::time_point::min();
        }
        else
        {
            time.tm_isdst = 0;
            time_t time_to_use = timegm(time);
            res = system_clock::from_time_t(time_to_use);
        }

        return res;
    }

    std::string get_content_type(const smooth::core::filesystem::Path& path)
    {
        // https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Complete_list_of_MIME_types
        const auto& ext = path.extension();

        if (ext == ".jpeg")
        {
            return "image/jpeg";
        }
        else if (ext == ".html")
        {
            return "text/html";
        }

        return "application/octet-stream";
    }

    time_t timegm(tm& tm)
    {
        static std::mutex timemg_lock{};
        std::lock_guard<std::mutex> lock(timemg_lock);

        time_t ret{};
        char* tz = nullptr;

        tz = getenv("TZ");
        setenv("TZ", "", 1);
        tzset();
        ret = std::mktime(&tm);

        if (tz)
        {
            setenv("TZ", tz, 1);
        }
        else
        {
            unsetenv("TZ");
        }

        tzset();

        return ret;
    }

    std::string http_method_to_string(const HTTPMethod m)
    {
        if (m == HTTPMethod::DELETE)
        {
            return "DELETE";
        }
        else if (m == HTTPMethod::GET)
        {
            return "GET";
        }
        else if (m == HTTPMethod::HEAD)
        {
            return "HEAD";
        }
        else if (m == HTTPMethod::PUT)
        {
            return "PUT";
        }
        else if (m == HTTPMethod::POST)
        {
            return "POST";
        }

        return "";
    }
}
