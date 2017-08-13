//
// Created by permal on 8/13/17.
//

#pragma once

namespace smooth
{
    namespace core
    {
        namespace util
        {
            // Until we get C++14...
            // Directly from Herb Sutter: https://herbsutter.com/gotw/_102/
            template<typename T, typename ...Args>
            std::unique_ptr <T> make_unique(Args&& ...args)
            {
                return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
            }
        }
    }
}


