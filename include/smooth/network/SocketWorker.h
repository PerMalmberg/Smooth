//
// Created by permal on 7/1/17.
//

#pragma once

#pragma once

#include <cstring>
#include <sys/socket.h>
#include <smooth/Task.h>

namespace smooth
{
    namespace network
    {
        template<uint32_t socket_count>
        class SocketWorker
                : smooth::Task
        {
            public:
                SocketWorker::SocketWorker(UBaseType_t priority)
                        : Task("SocketWorker", 4096, priority, 1, std::chrono::seconds(1))
                {
                }

                virtual ~SocketWorker()
                {
                }
        };
    }
}
