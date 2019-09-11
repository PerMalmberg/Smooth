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

#include <mutex>
#include <condition_variable>

namespace smooth::core::filesystem
{
    class FSLock
    {
        public:
            static void set_limit(int max_open_files);

            static int open_files();

            static int max_concurrently_opened();

            FSLock();

            virtual ~FSLock() final;

            FSLock(const FSLock&) = delete;

            FSLock(FSLock&&) = delete;

            FSLock& operator=(const FSLock&) = delete;

            FSLock& operator=(const FSLock&&) = delete;

        private:
            static std::mutex lock;
            static std::condition_variable cv;
            static int max;
            static int count;
            static int max_ever_opened;
    };
}
