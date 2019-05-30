#include <smooth/core/filesystem/FSLock.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

namespace smooth::core::filesystem
{

    std::mutex FSLock::lock{};
    std::condition_variable FSLock::cv{};
    int FSLock::max{0};
    int FSLock::count{0};
    int FSLock::max_ever_opened{0};

    void FSLock::init(int max_open_files)
    {
        std::unique_lock<std::mutex> guard{lock};
        max = max_open_files;
        Log::info("FSLock", Format("Max number of open files set to: {1}", Int32(max_open_files)));
    }

    int FSLock::open_files()
    {
        std::unique_lock<std::mutex> guard{lock};
        return count;
    }

    FSLock::FSLock()
    {
        std::unique_lock<std::mutex> guard{lock};
        cv.wait(guard, []{ return count < max; });

        count++;
        if(count > max_ever_opened)
        {
            max_ever_opened = count;
        }

        guard.unlock();
        cv.notify_one();
    }

    FSLock::~FSLock()
    {
        std::unique_lock<std::mutex> guard{lock};
        count--;
        cv.notify_one();
    }

    int FSLock::max_concurrently_opened()
    {
        std::unique_lock<std::mutex> guard{lock};
        return max_ever_opened;
    }
}