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
                    class ConnectEvent : public BaseEvent
                    {
                        public:
                            ConnectEvent() : BaseEvent(EventType::CONNECT)
                            {}
                    };
                }
            }
        }
    }
}
