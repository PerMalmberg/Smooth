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
