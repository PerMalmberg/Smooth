//
// Created by permal on 8/17/17.
//

#pragma once
#undef write
#include <driver/gpio.h>
#include "sdkconfig.h"

namespace smooth
{
    namespace core
    {
        namespace io
        {
            class BitBang
            {
                public:
                    void delay_100ns();
                private:
                    static const int cpu_frequency;
            };
        }
    }
}