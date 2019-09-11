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

#include <cstdint>

namespace smooth::core
{
    // These are the priorities used by the different tasks in the
    // Smooth framework. Keep your application tasks on a lower
    // priority than the framework services to ensure a functioning
    // system.
    const uint32_t APPLICATION_BASE_PRIO = 5;

    const uint32_t TIMER_SERVICE_PRIO = 19;
    const uint32_t SOCKET_DISPATCHER_PRIO = 20;
}
