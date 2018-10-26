//
// Created by permal on 8/5/17.
//

#pragma once

#include "BaseEvent.h"

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                namespace event
                {
                    class DisconnectEvent : public BaseEvent
                    {
                        public:
                            DisconnectEvent() : BaseEvent(EventType::DISCONNECT)
                            {}
                    };
                }
            }
        }
    }
}
