//
// Created by permal on 6/27/17.
//

#pragma once

#include "Queue.h"
#include "Link.h"
#include <smooth/ipc/IEventListener.h>

namespace smooth
{
    namespace ipc
    {
        template<typename T>
        class DirectEvent
                : public Link<T>, public Queue<T>
        {
            public:
                DirectEvent(const std::string& name, IEventListener <T>& listener)
                        :
                        Link<T>(),
                        Queue<T>(name, 1),
                        listener(listener)
                {
                    this->subscribe(this);
                }


                ~DirectEvent()
                {
                    this->unsubscribe(this);
                }

                bool push(const T& item) override
                {
                    listener.message(item);
                    return true;
                }

            private:
                IEventListener <T>& listener;
        };
    }
}