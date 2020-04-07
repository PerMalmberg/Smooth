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

/****************************************************************************************
 * Typical results from running program on M5Stack
 *
 * W (262499) APP: ============ APP TICK TICK  =============
 * I (262499) MemStat: Mem type |  8-bit free | Smallest block | Minimum free | 32-bit free | Smallest block | Minimum free
 * I (262506) MemStat: INTERNAL |      216740 |         113804 |       216048 |      273296 |         113804 |       272596
 * I (262517) MemStat:      DMA |      216740 |         113804 |       216048 |      216740 |         113804 |       216048
 * I (262529) MemStat:   SPIRAM |           0 |              0 |            0 |           0 |              0 |            0
 * I (262540) MemStat:
 * I (262543) MemStat:             Name |      Stack |  Min free stack |  Max used stack
 * I (262551) MemStat:         MainTask |      16384 |           12156 |            4228
 * I (262560) MemStat: SocketDispatcher |      20480 |           18392 |            2088
 * I (262568) APP: ........................................
 * E (262577) APP: DHT12 Temperature  (degC)  = 20.6
 * E (262580) APP: DHT12 Humidity     (%RH)   = 41.5
 * I (262585) APP: ........................................
 ****************************************************************************************/
#include <vector>
#include "i2c_dht12_test.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/SystemStatistics.h"

using namespace smooth::core;
using namespace std::chrono;
using namespace smooth::application::sensor;

namespace i2c_dht12_test
{
    static const char* TAG = "APP";

    App::App() : Application(APPLICATION_BASE_PRIO, seconds(4)),
            i2c_master(I2C_NUM_0,                       // I2C Port 0
                       GPIO_NUM_22,                     // SCL pin
                       false,                           // SCL internal pullup NOT enabled
                       GPIO_NUM_21,                     // SDA pin
                       false,                           // SDA internal pullup NOT enabled
                       100 * 1000)                      // clock frequency - 100kHz
    {
    }

    void App::init()
    {
        Application::init();

        dht12_initialized = init_i2c_dht12();
        Log::info(TAG, "DHT12 intialization --- {}", dht12_initialized ? "Succeeded" : "Failed");
    }

    void App::tick()
    {
        Log::warning(TAG, "============ APP TICK TICK  =============");
        SystemStatistics::instance().dump();
        Log::info(TAG, "........................................" );

        if (dht12_initialized)
        {
            print_sensor_measurements();
        }
    }

    bool App::init_i2c_dht12()
    {
        bool res = true;
        auto device = i2c_master.create_device<DHT12>(0x5C);   // DHT12 i2c address

        Log::info(TAG, "Scanning for DHT12");

        if (device->is_present())
        {
            Log::info(TAG, "DHT12 is present");
            sensor = std::move(device);
        }
        else
        {
            Log::error(TAG, "DHT12 is NOT present");
            res = false;
        }

        return res;
    }

    void App::print_sensor_measurements()
    {
        float temperature, humidity;
        sensor->read_measurements(humidity, temperature);

        Log::error(TAG, "DHT12 Temperature  (degC)  = {}", temperature);
        Log::error(TAG, "DHT12 Humidity     (%RH)   = {}", humidity);
        Log::info(TAG, "........................................" );
    }
}
