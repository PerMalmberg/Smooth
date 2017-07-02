//
// Created by permal on 7/2/17.
//

#pragma once

#include "Link.h"

namespace smooth
{
    namespace ipc
    {
        template <typename T>
        class Publisher
        {
            public:
                static void publish(T& item);
        };


        template<typename T>
        void Publisher<T>::publish(T& item)
        {
            Link<T>::publish( item );
        }
    }
}
