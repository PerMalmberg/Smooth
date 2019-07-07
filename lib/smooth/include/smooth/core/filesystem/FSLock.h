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

#include <mutex>
#include <condition_variable>

namespace smooth::core::filesystem
{
    class FSLock
    {
        public:
            static void init(int max_open_files);
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