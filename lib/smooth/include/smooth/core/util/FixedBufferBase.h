//
// Created by permal on 8/20/17.
//

#pragma once

namespace smooth
{
    namespace core
    {
        namespace util
        {
            template<typename T>
            class FixedBufferBase
            {
                public:
                    virtual size_t size() const = 0;
                    virtual T* data() = 0;
                private:

            };
        }
    }
}