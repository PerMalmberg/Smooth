//
// Created by permal on 7/13/17.
//

#pragma once

#include <string>

namespace smooth
{
    namespace timer
    {
        template<typename T>
        class Timer
        {
            public:
                Timer();

            protected:
                const std::string name;
                int id;
        };

        template<typename T>
        Timer<T>::Timer(const std::string& name, int id)
                : name(name), id(id)
        {
        }
    }
}
