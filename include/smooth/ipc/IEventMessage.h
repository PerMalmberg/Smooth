//
// Created by permal on 6/27/17.
//

#pragma once

namespace smooth
{
    namespace ipc
    {
        template<typename T>
        class IEventListener
        {
            public:
                virtual ~IEventListener()
                {
                }

                virtual void message(const T& msg) = 0;
        };
    }
}