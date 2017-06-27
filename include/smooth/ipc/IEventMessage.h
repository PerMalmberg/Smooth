//
// Created by permal on 6/27/17.
//

#pragma once

namespace smooth
{
    namespace ipc
    {
        template <typename T>
        class IEventMessage;

        template <typename T>
        class IEventListener
        {
            public:
                virtual void message( const IEventMessage<T>& msg) = 0;
                virtual void message( const T& msg) = 0;
        };

        template <typename T>
        class IEventMessage
        {
            public:
                virtual void execute( IEventListener<T>& listener ) const
                {
                    listener.message(*this);
                }
        };
    }
}