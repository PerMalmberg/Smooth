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
 * Typical results from running program on esp32-devkit v1
 *
 * W (74246) APP:: ============ APP TICK TICK  =============
 * I (74247) MemStat: Mem type |  8-bit free | Smallest block | Minimum free | 32-bit free | Smallest block | Minimum free
 * I (74253) MemStat: INTERNAL |      215796 |         113804 |       215448 |      273088 |         113804 |       272732
 * I (74265) MemStat:      DMA |      215796 |         113804 |       215448 |      215796 |         113804 |       215448
 * I (74276) MemStat:   SPIRAM |           0 |              0 |            0 |           0 |              0 |            0
 * I (74287) MemStat:
 * I (74290) MemStat:             Name |      Stack |  Min free stack |  Max used stack
 * I (74299) MemStat:         MainTask |      16384 |           12076 |            4308
 * I (74307) MemStat: SocketDispatcher |      20480 |           18380 |            2100
 * I (74315) APP:: ........................................
 * E (74322) APP:: cmd = 0x09  param [0] = 0xa4
 * E (74326) APP:: cmd = 0x09  param [1] = 0x53
 * E (74331) APP:: cmd = 0x09  param [2] = 0x04
 * E (74336) APP:: cmd = 0x09  param [3] = 0x00
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
 * E (74424) APP:: BME280 ID  = 0x60
 * I (74427) APP:: ........................................
 ****************************************************************************************/
#include <vector>
#include "spi_4_line_devices_test.h"
#include "smooth/application/display/ILI9341.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/SystemStatistics.h"

using namespace smooth::core;
using namespace std::chrono;
using namespace smooth::core::io::spi;
using namespace smooth::application::display;
using namespace smooth::application::sensor;

namespace spi_4_line_devices_test
{
    static const char* TAG = "APP";
    static const uint8_t READ_ID_INFO = 0x04;
    static const uint8_t READ_DISPLAY_STATUS = 0x09;
    static const uint8_t READ_DISPLAY_PWR_MODE = 0x0A;
    static const uint8_t READ_DISPLAY_MADCTL = 0x0B;
    static const uint8_t ONE_PARAM = 0x01;
    static const uint8_t TWO_PARAMS = 0x02;
    static const uint8_t THREE_PARAMS = 0x03;
    static const uint8_t FOUR_PARAMS = 0x04;

    App::App() : Application(APPLICATION_BASE_PRIO, seconds(3)),
            spi_host(VSPI_HOST),            // Use VSPI as host

            spi_master(
                spi_host,                   // host VSPI
                DMA_1,                      // use DMA
                GPIO_NUM_23,                // mosi gpio pin
                GPIO_NUM_19,                // miso gpio pin  (full duplex)
                GPIO_NUM_18,                // clock gpio pin
                0)                          // max transfer size default of 4096
    {
    }

    void App::init()
    {
        Application::init();

        ili9341_initialized = init_ILI9341();
        Log::info(TAG, "ILI9341 intialization --- {}", ili9341_initialized ? "Succeeded" : "Failed");

        bme280_initialized = init_BME280SPI();
        Log::info(TAG, "BME280 intialization --- {}", bme280_initialized ? "Succeeded" : "Failed");
    }

    void App::tick()
    {
        Log::warning(TAG, "============ APP TICK TICK  =============");
        SystemStatistics::instance().dump();
        Log::info(TAG, "........................................" );

        if (ili9341_initialized)
        {
            //print_display_parameters(READ_ID_INFO, THREE_PARAMS);
            print_display_parameters(READ_DISPLAY_STATUS, FOUR_PARAMS);

            //print_display_parameters(READ_DISPLAY_PWR_MODE, ONE_PARAM);
            //print_display_parameters(READ_DISPLAY_MADCTL, ONE_PARAM);
        }

        if (bme280_initialized)
        {
            print_thp_sensor_measurements();
            print_thp_sensor_configuration();
            print_thp_sensor_id();
        }
    }

    bool App::init_ILI9341()
    {
        auto device = spi_master.create_device<LCDSpi>(
                        GPIO_NUM_14,            // chip select gpio pin
                        GPIO_NUM_27,            // data command gpio pin
                        0,                      // spi command_bits
                        0,                      // spi address_bits,
                        0,                      // bits_between_address_and_data_phase,
                        0,                      // spi_mode = 0,
                        128,                    // spi positive_duty_cycle,
                        0,                      // spi cs_ena_posttrans,
                        SPI_MASTER_FREQ_16M,    // spi-sck = 16MHz
                        0,                      // full duplex (4-wire)
                        7,                      // queue_size,
                        true,                   // use pre-trans callback
                        true);                  // use post-trans callback

        Log::info(TAG, "Initializing of SPI Device: ILI9341");
        bool res = device->init(spi_host);

        if (res)
        {
            device->add_reset_pin(std::make_unique<DisplayPin>(GPIO_NUM_33, false, false, false));
            device->hw_reset(true, milliseconds(5), milliseconds(150));
            res &= device->send_init_cmds(ili9341_init_cmds_1.data(), ili9341_init_cmds_1.size());
            display = std::move(device);
        }
        else
        {
            Log::error(TAG, "Initializing of SPI Device: ILI9341 --- FAILED");
        }

        return res;
    }

    bool App::init_BME280SPI()
    {
        bool res = false;
        auto device = spi_master.create_device<BME280SPI>(
                        GPIO_NUM_13,            // chip select gpio pin
                        0,                      // spi command_bits
                        0,                      // spi address_bits,
                        0,                      // bits_between_address_and_data_phase,
                        0,                      // spi_mode = 0,
                        128,                    // spi positive_duty_cycle,
                        0,                      // spi cs_ena_posttrans,
                        SPI_MASTER_FREQ_10M,    // spi-sck = 10MHz
                        0,                      // full duplex (4-wire)
                        2,                      // queue_size,
                        false,                  // will not use pre-trans callback
                        false);                 // will not use post-trans callback

        Log::info(TAG, "Initializing of SPI Device: BME280SPI");

        if (device->init(spi_host))
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
            Log::error(TAG, "Initializing of SPI Device: BME280SPI --- FAILED");
        }

        thp_sensor = std::move(device);

        return res;
    }

    void App::print_display_parameters(const uint8_t cmd, uint8_t param_count)
    {
        std::vector<uint8_t> rxdata;
        display->read_params(cmd, rxdata, param_count);

        for (uint8_t i = 0; i < param_count; i++)
        {
            Log::error(TAG, "cmd = {:#04x}  param [{}] = {:#04x}", cmd, i, rxdata.at(i));
        }

        Log::info(TAG, "........................................" );
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
