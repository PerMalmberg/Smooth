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
#include "smooth/application/display/LCDSpi.h"
#include "smooth/application/io/spi/BME280SPI.h"

namespace spi_4_line_devices_test
{
    class App : public smooth::core::Application
    {
        public:
            App();

            void init() override;

            void tick() override;

            void print_display_parameters(const uint8_t cmd, uint8_t param_count);

            void print_thp_sensor_measurements();

            void print_thp_sensor_configuration();

            void print_thp_sensor_id();

        private:
            bool init_ILI9341();

            bool init_BME280SPI();

            spi_host_device_t spi_host;
            std::unique_ptr<smooth::application::display::LCDSpi> display{};
            std::unique_ptr<smooth::application::sensor::BME280SPI> thp_sensor{};
            bool ili9341_initialized{ false };
            bool bme280_initialized{ false };
    };
}
