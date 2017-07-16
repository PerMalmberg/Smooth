//
// Created by permal on 7/16/17.
//

#pragma once

#include <string>
#include "esp_attr.h"

namespace smooth
{
    namespace timer
    {
        class ITimer
        {
            public:
                virtual ~ITimer(){}
                virtual void start() = 0;
                virtual IRAM_ATTR void start_from_isr() = 0;
                virtual void stop() = 0;
                virtual IRAM_ATTR void stop_from_isr() = 0;
                virtual void reset() = 0;
                virtual IRAM_ATTR void reset_from_isr() = 0;
                virtual int get_id() = 0;
                virtual const std::string& get_name() = 0;
        };
    }
}
