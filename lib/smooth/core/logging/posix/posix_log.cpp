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

#include <smooth/core/logging/log.h>
#include <iostream>
#include <mutex>

namespace smooth::core
{
    namespace logging
    {
        static std::mutex guard;

        void Log::error(const std::string& tag, const Format& fmt)
        {
            std::lock_guard<std::mutex> lock(guard);
            std::cout << "(E)" << tag << ": " << fmt << std::endl;
        }

        void Log::warning(const std::string& tag, const Format& fmt)
        {
            std::lock_guard<std::mutex> lock(guard);
            std::cout << "(W)" << tag << ": " << fmt << std::endl;
        }

        void Log::info(const std::string& tag, const Format& fmt)
        {
            std::lock_guard<std::mutex> lock(guard);
            std::cout << "(I)" << tag << ": " << fmt << std::endl;
        }

        void Log::debug(const std::string& tag, const Format& fmt)
        {
            std::lock_guard<std::mutex> lock(guard);
            std::cout << "(D)" << tag << ": " << fmt << std::endl;
        }

        void Log::verbose(const std::string& tag, const Format& fmt)
        {
            std::lock_guard<std::mutex> lock(guard);
            std::cout << "(V)" << tag << ": " << fmt << std::endl;
        }
    }
}
