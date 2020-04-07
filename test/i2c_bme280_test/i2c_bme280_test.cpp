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
 * Typical results from running program esp32-DevKit-v1
 *
 * W (74246) APP:: ============ APP TICK TICK  =============
 * I (74247) MemStat: Mem type |  8-bit free | Smallest block | Minimum free | 32-bit free | Smallest block | Minimum free
 * I (74253) MemStat: INTERNAL |      216840 |         113804 |       216556 |      273556 |         113804 |       273264
 * I (74265) MemStat:      DMA |      216840 |         113804 |       216556 |      216840 |         113804 |       216556
 * I (74276) MemStat:   SPIRAM |           0 |              0 |            0 |           0 |              0 |            0
 * I (74287) MemStat:
 * I (74290) MemStat:             Name |      Stack |  Min free stack |  Max used stack
 * I (74299) MemStat:         MainTask |      16384 |           12084 |            4300
 * I (74307) MemStat: SocketDispatcher |      20480 |           18388 |            2092
 * I (74341) APP:: ........................................
 * E (74348) APP:: BME280 Temperature  (degC)  = 22.4
 * E (74353) APP:: BME280 Humidity     (%RH)   = 37.0
 * E (74358) APP:: BME280 Pressure     (hPa)   = 933.411
 * E (74364) APP:: Barometric Pressure (in Hg) = 27.5636
 * I (74370) APP:: ........................................
 * E (74376) APP:: Mode configuration          = Normal
 * E (74381) APP:: Temperature configuration   = Oversamplingx1
 * E (74387) APP:: Humidity configuration      = Oversamplingx1
 * E (74394) APP:: Pressure configuration      = Oversamplingx1
 * E (74400) APP:: Standby configuration       = ST_1000
 * E (74406) APP:: Filter configuration        = FC_OFF
 * E (74412) APP:: Spi Interface configuration = SPI_4_WIRE
 * I (74417) APP:: ........................................
 ****************************************************************************************/
#include <vector>
#include "i2c_bme280_test.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/SystemStatistics.h"

using namespace smooth::core;
using namespace std::chrono;
using namespace smooth::application::sensor;

namespace i2c_bme280_test
{
    static const char* TAG = "APP";

    App::App() : Application(APPLICATION_BASE_PRIO, seconds(3)),
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

        bme280_initialized = init_BME280I2C();
        Log::info(TAG, "BME280 initialization --- {}", bme280_initialized ? "Succeeded" : "Failed");
    }

    void App::tick()
    {
        Log::warning(TAG, "============ APP TICK TICK  =============");
        SystemStatistics::instance().dump();
        Log::info(TAG, "........................................" );

        if (bme280_initialized)
        {
            print_thp_sensor_measurements();
            print_thp_sensor_configuration();
        }
    }

    bool App::init_BME280I2C()
    {
        bool res = false;
        auto device = i2c_master.create_device<BME280>(0x76);   // BME280 i2c address

        Log::info(TAG, "Scanning for BME280");

        if (device->is_present())
        {
            Log::info(TAG, "BME280 reset: {}", device->reset());

            bool measuring = false;
            bool loading_from_nvm = false;

            while (!device->read_status(measuring, loading_from_nvm) || loading_from_nvm)
            {
                Log::info(TAG, "Waiting for BME280 to complete reset operation... {} {}", measuring, loading_from_nvm);
            }

            res = device->configure_sensor(BME280Core::SensorMode::Normal,
                                           BME280Core::OverSampling::Oversamplingx1,
                                           BME280Core::OverSampling::Oversamplingx1,
                                           BME280Core::OverSampling::Oversamplingx1,
                                           BME280Core::StandbyTimeMS::ST_1000,
                                           BME280Core::FilterCoeff::FC_OFF);

            Log::info(name, "Configure BME280: {}", res);

            if (res)
            {
                Log::info(TAG, "BME280 initialized, ID: {}", device->read_id());
            }
            else
            {
                Log::error(name, "Could not init BME280");
            }
        }
        else
        {
            Log::error(TAG, "BME280 not present");
        }

        thp_sensor = std::move(device);

        return res;
    }

    void App::print_thp_sensor_measurements()
    {
        float temperature, humidity, pressure;
        thp_sensor->read_measurements(humidity, pressure, temperature);

        Log::error(TAG, "BME280 Temperature  (degC)  = {}", temperature);
        Log::error(TAG, "BME280 Humidity     (%RH)   = {}", humidity);
        Log::error(TAG, "BME280 Pressure     (hPa)   = {}", pressure / 100);
        Log::error(TAG, "Barometric Pressure (in Hg) = {}", pressure / static_cast<float> (3386.389));
        Log::info(TAG, "........................................" );
    }

    void App::print_thp_sensor_configuration()
    {
        BME280Core::SensorMode mode;
        BME280Core::OverSampling humidity;
        BME280Core::OverSampling pressure;
        BME280Core::OverSampling temperature;
        BME280Core::StandbyTimeMS standby;
        BME280Core::FilterCoeff filter;
        BME280Core::SpiInterface spi_interface;

        thp_sensor->read_configuration(mode, humidity, pressure, temperature, standby, filter, spi_interface);

        Log::error(TAG, "Mode configuration          = {}", BME280Core::SensorModeStrings[mode] );
        Log::error(TAG, "Temperature configuration   = {}", BME280Core::OverSamplingStrings[temperature] );
        Log::error(TAG, "Humidity configuration      = {}", BME280Core::OverSamplingStrings[humidity] );
        Log::error(TAG, "Pressure configuration      = {}", BME280Core::OverSamplingStrings[pressure] );
        Log::error(TAG, "Standby configuration       = {}", BME280Core::StandbyTimesStrings[standby] );
        Log::error(TAG, "Filter configuration        = {}", BME280Core::FilterCoeffStrings[filter] );
        Log::error(TAG, "Spi Interface configuration = {}", BME280Core::SpiInterfaceStrings[spi_interface] );
        Log::info(TAG, "........................................" );
    }

    void App::print_thp_sensor_id()
    {
        Log::error(TAG, "BME280 ID  = {:#04x}", thp_sensor->read_id());
        Log::info(TAG, "........................................" );
    }
}
