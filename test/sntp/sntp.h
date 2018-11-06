#pragma once

#include <smooth/core/Application.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/sntp/Sntp.h>
#include <iostream>

namespace sntp
{
    class App
            : public smooth::core::Application
    {
        public:
            App();

            void init() override;

            void tick() override;

        private:
            smooth::core::sntp::Sntp sntp;

            void print_time() const;
    };
}