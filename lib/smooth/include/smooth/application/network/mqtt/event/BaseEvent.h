//
// Created by permal on 8/5/17.
//

#pragma once

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
                    class BaseEvent
                    {
                        public:
                            enum EventType
                            {
                                CONNECT,
                                DISCONNECT
                            };

                            BaseEvent() = default;

                            explicit BaseEvent(EventType event_type)
                                    : type(event_type)
                            {
                            }

                            EventType get_type() const
                            {
                                return type;
                            }

                        private:
                            EventType type;
                    };
                }
            }
        }
    }
}
