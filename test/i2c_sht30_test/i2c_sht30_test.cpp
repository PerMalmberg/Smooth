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

#include "i2c_sht30_test.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core;
using namespace std::chrono;
using namespace smooth::application::sensor;

namespace i2c_sht30_test
{
    static const char* TAG = "APP";

    App::App() : Application(APPLICATION_BASE_PRIO, seconds(4)),
            i2c_master(I2C_NUM_0,                            // I2C Port 0
                       GPIO_NUM_22,                          // SCL pin
                       false,                                // SCL internal pullup NOT enabled
                       GPIO_NUM_21,                          // SDA pin
                       false,                                // SDA internal pullup NOT enabled
                       100 * 1000)                           // clock frequency - 100kHz
    {
    }

    void App::init()
    {
        Application::init();

        sht30_initialized = init_i2c_sht30();
        Log::info(TAG, "SHT30 intialization --- {}", sht30_initialized ? "Succeeded" : "Failed");
    }

    void App::tick()
    {
        Log::warning(TAG, "============ APP TICK TICK  =============");

        if (sht30_initialized)
        {
            print_sensor_measurements();
        }
    }

    bool App::init_i2c_sht30()
    {
        bool res = true;
        auto device = i2c_master.create_device<SHT30>(0x44);   // SHT30 i2c address

        Log::info(TAG, "Scanning for SHT30");

        if (device->is_present())
        {
            Log::info(TAG, "SHT30 is present");
            sensor = std::move(device);
        }
        else
        {
            Log::error(TAG, "SHT30 is NOT present");
            res = false;
        }

        return res;
    }

    void App::print_sensor_measurements()
    {
        float temperature, humidity;
        sensor->read_measurements(humidity, temperature);

        Log::error(TAG, "SHT30 Temperature  (degC)  = {}", temperature);
        Log::error(TAG, "SHT30 Humidity     (%RH)   = {}", humidity);
        Log::info(TAG, "........................................" );
    }
}
