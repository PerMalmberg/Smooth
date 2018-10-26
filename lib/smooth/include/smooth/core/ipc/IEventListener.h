//
// Created by permal on 6/27/17.
//

#pragma once

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            /// Implement this interface for each type of event you wish to receive from a TaskEventQueue.
            /// \tparam T
            template<typename EventType>
            class IEventListener
            {
                public:
                    virtual ~IEventListener()
                    {
                    }

                    /// The response method where the event will be received from a TaskEventQueue<EventType>
                    /// \param event The event
                    virtual void event(const EventType& event) = 0;
            };
        }
    }
}