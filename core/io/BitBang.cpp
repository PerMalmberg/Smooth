//
// Created by permal on 8/17/17.
//

#include <smooth/core/io/BitBang.h>

namespace smooth
{
    namespace core
    {
        namespace io
        {
#if CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ == 240
            const int BitBang::cpu_frequency = 240;
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ == 160
            const int BitBang::cpu_frequency = 160;
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ == 80
            const int BitBang::cpu_frequency = 80;
#else
#error "The selected CPU frequency is not supported"
#endif

            void BitBang::delay_100ns()
            {
                __asm__ __volatile__("nop");
            }

        }
    }
}