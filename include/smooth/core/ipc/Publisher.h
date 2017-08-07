//
// Created by permal on 7/2/17.
//

#pragma once

#include "Link.h"

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            /// Publisher for messages of type T
            /// \tparam T The type to publish.
            template<typename T>
            class Publisher
            {
                public:
                    /// Publishes a copy of the provided item to all subscribers that are registered for it
                    /// in a thread-safe manner.
                    static void publish(T& item);
            };


            template<typename T>
            void Publisher<T>::publish(T& item)
            {
                Link<T>::publish(item);
            }
        }
    }
}
