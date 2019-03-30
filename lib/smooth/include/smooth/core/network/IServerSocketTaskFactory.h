#pragma once

#include <smooth/core/Task.h>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            class IServerSocketTaskFactory
            {
                public:
                    virtual smooth::core::Task& get_task() = 0;
            };
        }
    }
}