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
#include <mutex>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <fmt/format.h>
#pragma GCC diagnostic pop
#include <iostream>
#include "esp_log.h"

namespace smooth::core::logging
{
    class Log
    {
        public:
            static std::mutex guard;

            static fmt::basic_memory_buffer<char, 500> buff;

            template<typename ...Args>
            static void log(const char level, const std::string& tag, Args&&...args)
            {
                std::unique_lock<std::mutex> lock(guard);
                buff.clear();
                fmt::format_to(buff, args...);
                std::cout << "(" << level << ")" << tag << ": " << fmt::to_string(buff) << std::endl;
            }

            template<typename ...Args>
            static void error(const std::string&& tag, const std::string&& fmt, Args...args)
            {
                log('E', tag, fmt, args...);
            }

            template<typename ...Args>
            static void warning(const std::string& tag, Args&&...args)
            {
                log('W', tag, args...);
            }

            template<typename ...Args>
            static void info(const std::string& tag, Args&&...args)
            {
                log('I', tag, args...);
            }

            template<typename ...Args>
            static void debug(const std::string& tag, Args&&...args)
            {
                log('D', tag, args...);
            }

            template<typename ...Args>
            static void verbose(const std::string& tag, Args&&...args)
            {
                log('V', tag, args...);
            }
    };
}
