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

#include "smooth/core/filesystem/FSLock.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;

namespace smooth::core::filesystem
{
    std::mutex FSLock::lock{};
    std::condition_variable FSLock::cv{};
    int FSLock::max{ 0 };
    int FSLock::count{ 0 };
    int FSLock::max_ever_opened{ 0 };

    void FSLock::set_limit(int max_open_files)
    {
        std::unique_lock<std::mutex> guard{ lock };
        max = max_open_files;
        Log::info("FSLock", "Max number of open files set to: {}", max_open_files);
    }

    int FSLock::open_files()
    {
        std::unique_lock<std::mutex> guard{ lock };

        return count;
    }

    FSLock::FSLock()
    {
        std::unique_lock<std::mutex> guard{ lock };

        if (max <= 0)
        {
            throw std::invalid_argument("Must call FSLock::set_limit() before using FSLock");
        }

        cv.wait(guard, [] { return count < max; });

        count++;

        if (count > max_ever_opened)
        {
            max_ever_opened = count;
        }

        guard.unlock();
        cv.notify_one();
    }

    FSLock::~FSLock()
    {
        std::unique_lock<std::mutex> guard{ lock };
        count--;
        cv.notify_one();
    }

    int FSLock::max_concurrently_opened()
    {
        std::unique_lock<std::mutex> guard{ lock };

        return max_ever_opened;
    }
}
