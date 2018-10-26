#pragma once

#include <smooth/core/Application.h>
#include <smooth/core/task_priorities.h>
#include <iostream>

namespace logging
{
    class App : public smooth::core::Application
    {
    public:
        App();

        void init() override;
        void tick() override;
    };
}