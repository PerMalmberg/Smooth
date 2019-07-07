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
