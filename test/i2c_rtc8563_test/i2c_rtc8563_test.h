/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma once

#include "smooth/core/Application.h"
#include "smooth/core/task_priorities.h"
#include "smooth/core/io/i2c/Master.h"
#include "smooth/application/io/i2c/PCF8563.h"

namespace i2c_rtc8563_test
{
    class App : public smooth::core::Application
    {
        public:
            App();

            void init() override;

            void tick() override;

            void set_time();

            void get_time();

            void set_alarm();

            void get_alarm();

            bool is_alarm_active();

            void clear_alarm_active();

        private:
            void init_i2c_rtc8563();

            smooth::core::io::i2c::Master i2c0_master;
            std::unique_ptr<smooth::application::sensor::PCF8563> rtc8563{};
            bool rtc8563_initialized{ false };
            int alarm_active_count{ 0 };
    };
}
