//
// Created by permal on 10/29/17.
//

#pragma

#include <cstdint>

namespace smooth
{
    namespace core
    {
        // These are the priorities used by the different tasks in the
        // Smooth framework. Keep your application tasks on a lower
        // priority than the framework services to ensure a functioning
        // system.
        const uint32_t APPLICATION_BASE_PRIO = 5;

        const uint32_t TIMER_SERVICE_PRIO = 19;
        const uint32_t SOCKET_DISPATCHER_PRIO = 20;
    }
}
