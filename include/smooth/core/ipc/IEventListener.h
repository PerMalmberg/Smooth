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
            /// Inherit from this class to allow TaskEventQueue to send event to your subclass.
            /// \tparam T
            template<typename EventType>
            class IEventListener
            {
                public:
                    virtual ~IEventListener()
                    {
                    }

                    /// The event method where the event message will be received.
                    /// Override this in your subclass.
                    /// \param event
                    virtual void event(const EventType& event) = 0;
            };
        }
    }
}