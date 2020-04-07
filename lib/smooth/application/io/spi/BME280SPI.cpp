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
#include <cstring>
#include "smooth/core/logging/log.h"
#include "smooth/application/io/spi/BME280SPI.h"

using namespace smooth::core::logging;
using namespace smooth::core;
using namespace smooth::core::io::spi;

namespace smooth::application::sensor
{
    static const char* TAG = "BME280SPI";

    BME280SPI::BME280SPI(std::mutex& guard,
                         gpio_num_t chip_select_pin,
                         uint8_t spi_command_bits,
                         uint8_t spi_address_bits,
                         uint8_t bits_between_address_and_data_phase,
                         uint8_t spi_mode,
                         uint8_t spi_positive_duty_cycle,
                         uint8_t spi_cs_ena_posttrans,
                         int spi_clock_speed_hz,
                         uint32_t spi_device_flags,
                         int spi_queue_size,
                         bool use_pre_transaction_callback,
                         bool use_post_transaction_callback)
            : SPIDevice(guard,
                        spi_command_bits,
                        spi_address_bits,
                        bits_between_address_and_data_phase,
                        spi_mode,
                        spi_positive_duty_cycle,
                        spi_cs_ena_posttrans,
                        spi_clock_speed_hz,
                        spi_device_flags,
                        spi_queue_size,
                        use_pre_transaction_callback,
                        use_post_transaction_callback),

              cs_pin(chip_select_pin)
    {
    }

    bool BME280SPI::init(spi_host_device_t host)
    {
        auto res = initialize(host, cs_pin);

        return res;
    }

    bool BME280SPI::read(const uint8_t bme280_reg, uint8_t* rxdata, size_t length)
    {
        std::lock_guard<std::mutex> lock(get_guard());
        spi_transaction_t trans;
        std::memset(&trans, 0, sizeof(trans));  //Zero out the transaction
        trans.rx_buffer = rxdata;
        trans.rxlength = 8 * length;
        trans.length = 8 * length;
        trans.tx_buffer = &bme280_reg;

        auto res = polling_write(trans);

        return res;
    }

    bool BME280SPI::write(const uint8_t* txdata, size_t length)
    {
        std::lock_guard<std::mutex> lock(get_guard());
        spi_transaction_t trans;
        std::memset(&trans, 0, sizeof(trans));  //Zero out the transaction
        trans.rx_buffer = nullptr;
        trans.length = 8 * length;
        trans.tx_buffer = const_cast<uint8_t*>(txdata);

        auto res = polling_write(trans);

        return res;
    }

    uint8_t BME280SPI::read_id()
    {
        auto res = read(BME280Core::ID_REG, rxdata.data(), 2);

        return res ? rxdata[1] : 0;
    }

    bool BME280SPI::reset()
    {
        SpiDmaFixedBuffer<uint8_t, 4> txdata;

        // for spi write bit 7 of register address must be zero
        // modify 0xE0 to 0x60;
        txdata[0] = BME280Core::RESET_REG& 0x7F;

        // magic number to the reset device
        txdata[1] = 0xB6;

        return write(txdata.data(), 2);
    }

    bool BME280SPI::read_status(bool& is_measuring, bool& updating_from_nvm)
    {
        auto res = read(BME280Core::STATUS_REG, rxdata.data(), 2);

        if (res)
        {
            is_measuring = rxdata[1] & 0x4;
            updating_from_nvm = rxdata[1] & 0x1;
        }

        return res;
    }

    bool BME280SPI::configure_sensor(BME280Core::SensorMode mode,
                                     BME280Core::OverSampling humidity,
                                     BME280Core::OverSampling pressure,
                                     BME280Core::OverSampling temperature,
                                     BME280Core::StandbyTimeMS standby,
                                     BME280Core::FilterCoeff filter_coeff,
                                     BME280Core::SpiInterface spi_interface)
    {
        std::vector<uint8_t> datagram{};

        bme280_core.get_configure_sensor_datagram(datagram,
                                             mode,
                                             humidity,
                                             pressure,
                                             temperature,
                                             standby,
                                             filter_coeff,
                                             spi_interface);

        // Since we chose to use DMA on this SPI Device use SpiDmaFixedBuffer for the tx_buffer in spi_transfer
        SpiDmaFixedBuffer<uint8_t, 8> txdata{};

        // for spi write the bit in position b7 of register address must be zero
        txdata[0] = datagram.at(0) & 0x7F;  // modify 0xF2 to 0x72
        txdata[1] = datagram.at(1);         // keep data the same
        txdata[2] = datagram.at(2) & 0x7F;  // modify 0xF4 to 0x74
        txdata[3] = datagram.at(3);         // keep data the same
        txdata[4] = datagram.at(4) & 0x7F;  // modify 0xF5 to 0x75
        txdata[5] = datagram.at(5);         // keep data the same

        return write(txdata.data(), 6);
    }

    bool BME280SPI::read_configuration(BME280Core::SensorMode& mode,
                                       BME280Core::OverSampling& humidity,
                                       BME280Core::OverSampling& pressure,
                                       BME280Core::OverSampling& temperature,
                                       BME280Core::StandbyTimeMS& standby,
                                       BME280Core::FilterCoeff& filter,
                                       BME280Core::SpiInterface& spi_interface)
    {
        // Since we chose to use DMA on this SPI Device use SpiDmaFixedBuffer for the rx_buffer in spi_transfer
        SpiDmaFixedBuffer<uint8_t, 4> ctrl_hum_data;
        SpiDmaFixedBuffer<uint8_t, 4> ctrl_meas_data;
        SpiDmaFixedBuffer<uint8_t, 4> config_data;

        auto res = read(BME280Core::CTRL_HUM_REG, ctrl_hum_data.data(), 2)
                   && read(BME280Core::CTRL_MEAS_REG, ctrl_meas_data.data(), 2)
                   && read(BME280Core::CONFIG_REG, config_data.data(), 2);

        if (res)
        {
            // Since we are using 4-line spi (full duplex) the sck clocks both data out
            // and data in at the same time.  The first byte going out is the register we want
            // to read and so the first byte received is garbage or dummy byte.  So we
            // will pass the 2nd byte received to get_configuration_settings.
            bme280_core.get_configuration_settings(ctrl_hum_data[1],
                                                   ctrl_meas_data[1],
                                                   config_data[1],
                                                   mode,
                                                   humidity,
                                                   pressure,
                                                   temperature,
                                                   standby,
                                                   filter,
                                                   spi_interface);
        }

        return res;
    }

    bool BME280SPI::read_trimming_parameters()
    {
        if (!trimming_read)
        {
            // Since we chose to use DMA on this SPI Device use SpiDmaFixedBuffer for the rx_buffer in spi_transfer
            SpiDmaFixedBuffer<uint8_t, 32> calib00_calib25_data{};    // 0x88-0xA1
            SpiDmaFixedBuffer<uint8_t, 8> calib26_calib32_data{};     // 0xE1-0xE7
            core::util::FixedBuffer<uint8_t, 32> calibration_data{};

            trimming_read = read(BME280Core::CALIB00_REG, calib00_calib25_data.data(), 27)
                            && read(BME280Core::CALIB26_REG, calib26_calib32_data.data(), 8);

            if (trimming_read)
            {
                // Since we are using 4-line spi (full duplex) the sck clocks both data out
                // and data in at the same time.  The first byte going out is the register we want
                // to read and so the first byte received is garbage or dummy byte.  So we
                // don't want to copy the dummy byte but copy 0x88-0x9F into calibration_data
                for (size_t i = 0; i < 24; i++)
                {
                    calibration_data[i] = calib00_calib25_data[i + 1];
                }

                // copy 0xA1 but not 0xA0 into calibration_data
                calibration_data[24] = calib00_calib25_data[25];

                // don't copy dummy byte but copy 0xE1 to 0xE7 into calibration_data
                for (size_t i = 0; i < 7; i++)
                {
                    calibration_data[i + 25] = calib26_calib32_data[i + 1];
                }

                bme280_core.populate_trimming_registers(calibration_data);
            }
            else
            {
                Log::error(TAG, "Problems reading trimming parameters");
            }
        }

        return trimming_read;
    }

    bool BME280SPI::read_measurements(float& humidity, float& pressure, float& temperature)
    {
        smooth::core::util::FixedBuffer<uint8_t, 8> measurement_data;

        bool res = read_trimming_parameters()
                   && read(BME280Core::PRESS_MSB_REG, rx_measurement_data.data(), 9);

        if (res)
        {
            // Since we are using 4-line spi (full duplex) the sck clocks both data out
            // and data in at the same time.  The first byte going out is the register we want
            // to read and so the first byte received is garbage or dummy byte.  So we
            // don't want to copy the dummy byte but copy actual data into measurement_data
            for (size_t i = 0; i < 8; i++)
            {
                measurement_data[i] = rx_measurement_data[i + 1];
            }

            bme280_core.get_measurements(measurement_data, humidity, pressure, temperature);
        }

        return res;
    }
}
