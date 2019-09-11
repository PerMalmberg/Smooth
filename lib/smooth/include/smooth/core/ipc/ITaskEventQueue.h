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

namespace smooth::core::ipc
{
    class QueueNotification;

    /// Common interface for TaskEventQueue
    /// As an application programmer you are not meant to call any of these methods.
    class ITaskEventQueue
    {
        public:
            ITaskEventQueue() = default;

            virtual ~ITaskEventQueue() = default;

            /// Forwards the next event to the event listener
            virtual void forward_to_event_listener() = 0;

            /// Returns the size of the event queue.
            virtual int size() = 0;

            virtual void register_notification(QueueNotification* notification) = 0;
    };
}
