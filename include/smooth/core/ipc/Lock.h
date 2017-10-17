//
// Created by permal on 6/26/17.
//

#pragma once

#include <chrono>

#undef bind

#include <mutex>

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            /// Class used to do scoped-based locking of a std::mutex.
            class Lock
            {
                public:
                    Lock(std::mutex& m)
                            : m(m)
                    {
                        m.lock();
                    }

                    ~Lock()
                    {
                        m.unlock();
                    }

                private:
                    std::mutex& m;
            };
        };

    }
}