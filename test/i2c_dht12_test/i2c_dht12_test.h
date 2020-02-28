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
#include "smooth/application/io/i2c/DHT12.h"

namespace i2c_dht12_test
{
    class App : public smooth::core::Application
    {
        public:
            App();

            void init() override;

            void tick() override;

            void print_sensor_measurements();

        private:
            bool init_i2c_dht12();

            smooth::core::io::i2c::Master i2c_master;
            std::unique_ptr<smooth::application::sensor::DHT12> sensor{};
            bool dht12_initialized{ false };
    };
}
