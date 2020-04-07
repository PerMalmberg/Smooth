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

/************************************************************************************
   SPECIAL NOTES                SPECIAL NOTES                          SPECIAL NOTES

   NOTE 1: This driver is written to work 4-wire SPI interface see page 32 of the
   datasheet.

   NOTE 2: The BME280 requires special handling when writing to the device.
   To write to a particular register the high order bit (b7) of the register address
   must be 0.  See figures 11 and 12 on page 33 of datasheet.
  ************************************************************************************/
#pragma once

#include "smooth/core/io/spi/Master.h"
#include "smooth/core/io/spi/SPIDevice.h"
#include "smooth/application/io/spi/BME280Core.h"
#include "smooth/core/io/spi/SpiDmaFixedBuffer.h"

namespace smooth::application::sensor
{
    class BME280SPI : public core::io::spi::SPIDevice
    {
        public:
            BME280SPI(std::mutex& guard,
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
                      bool use_post_transaction_callback);

            /// Initialize
            bool init(spi_host_device_t host);

            /// Reads the ID of the device
            uint8_t read_id();

            /// Resets the device
            bool reset();

            /// Configures the sensor
            /// \param mode The mode setting
            /// \param humidity The humidity oversampling setting
            /// \param pressure The pressure oversampling setting
            /// \param temperature The temperature oversampling setting
            /// \param standby The standby time setting
            /// \param filter_coeff The filter coefficient setting
            /// \param spi_interface The spi interface setting
            /// \return true on success, false on failure
            bool configure_sensor(BME280Core::SensorMode mode,
                                  BME280Core::OverSampling humidity,
                                  BME280Core::OverSampling pressure,
                                  BME280Core::OverSampling temperature,
                                  BME280Core::StandbyTimeMS standby,
                                  BME280Core::FilterCoeff filter_coeff,
                                  BME280Core::SpiInterface spi_interface = BME280Core::SpiInterface::SPI_4_WIRE);

            /// Reads the current configuration
            /// \param mode The mode configuration setting will be written to this address
            /// \param humidity The humidity configuration setting will be written to this address
            /// \param pressure The pressure configuration setting will be written to this address
            /// \param temperature The temperature configuration setting will be written to this address
            /// \param standby The standby times configuration setting will be written to this address
            /// \param filter_coeff The filter coefficient configuration setting will be written to this address
            /// \param spi_interface The spi_interface configuration setting will be written to this address
            /// \return true on success, false on failure
            bool read_configuration(BME280Core::SensorMode& mode,
                                    BME280Core::OverSampling& humidity,
                                    BME280Core::OverSampling& pressure,
                                    BME280Core::OverSampling& temperature,
                                    BME280Core::StandbyTimeMS& standby,
                                    BME280Core::FilterCoeff& filter,
                                    BME280Core::SpiInterface& spi_interface);

            /// Reads the status of the device
            /// \param is_measuring true if the device is doing conversions.
            /// \param updating_from_nvm true if data is being copied from NVM to image registers.
            /// \return true if status could be read, otherwise false.
            bool read_status(bool& is_measuring, bool& updating_from_nvm);

            /// Reads the current measurements
            /// \param humidity Humidity, in %RH
            /// \param pressure Pressure, in Pa
            /// \param temperature Temperature in degree Celsius
            /// \return true on success, false on failure.
            bool read_measurements(float& humidity, float& pressure, float& temperature);

        private:
            /// Pre Transmission Action
            /// The operations that need to be performed before the spi transaction is started
            void pre_transmission_action() override
            {
            }

            /// Post Transmission Action
            /// The operations that need to be performed after the spi transaction has ended
            void post_transmission_action() override
            {
            }

            /// Write
            /// \param txdata A pointer to the first element of the transmit data
            /// \param length The number of bytes in txdata
            bool write(const uint8_t* txdata, size_t length);

            /// Read
            /// \param bme280_reg The BME280 register we wnat to read
            /// \param rxdata A pointer to container that will hold the receive data
            /// \param length The number of bytes that will be read
            bool read(const uint8_t bme280_reg, uint8_t* rxdata, size_t length);

            /// Read trimming parameters; will be used to calculated compensated measurements
            bool read_trimming_parameters();

            gpio_num_t cs_pin;
            bool trimming_read{ false };
            smooth::application::sensor::BME280Core bme280_core{};
            smooth::core::io::spi::SpiDmaFixedBuffer<uint8_t, 4> rxdata;
            smooth::core::io::spi::SpiDmaFixedBuffer<uint8_t, 12> rx_measurement_data;
    };
}
