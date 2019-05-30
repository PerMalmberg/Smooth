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