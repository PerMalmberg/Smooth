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
            /// Inherit from this class to allow events to be sent to your subclass.
            /// \tparam T
            template<typename EventType>
            class IEventListener
            {
                public:
                    virtual ~IEventListener()
                    {
                    }

                    virtual void event(const EventType& event) = 0;
            };
        }
    }
}