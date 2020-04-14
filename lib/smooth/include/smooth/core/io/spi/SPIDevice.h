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

#include <mutex>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <driver/gpio.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#pragma GCC diagnostic pop

namespace smooth::core::io::spi
{
    /// Base class for all SPI devices
    class SPIDevice
    {
        public:
            /// Base constructor for SPI devices.
            /// @note These arguments corresponds to those in the spi_device_interface_config_t structure.
            /// \param guard The guard mutex (owned by the SPI Master) used to synchronize SPI transmisions between
            // devices.
            /// \param spi_command_bits Number of bits in command phase (0-16)
            /// \param spi_address_bits Number of bits in address phase (0-64)
            /// \param bits_between_address_and_data_phase Number of bits to insert between address and data phase
            /// \param spi_mode SPI mode (0-3)
            /// \param spi_positive_duty_cycle Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty).
            // Setting this to 0 (=not setting it) is equivalent to setting this to 128.
            /// \param spi_cs_ena_posttrans Amount of SPI bit-cycles the cs should stay active after the transmission
            // (0-16)
            /// \param spi_clock_speed_hz Clock speed, in Hz
            /// \param spi_device_flags Bitwise OR of SPI_DEVICE_* flags
            /// \param spi_queue_size Transaction queue size. This sets how many transactions can be 'in the air'
            // (queued using spi_device_queue_trans but not yet finished using spi_device_get_trans_result) at the same
            // time
            /// \param use_pre_transaction_callback If true the pre_transaction_callback will be used.
            /// \param use_post_transaction_callback If true the post_transaction_callback will be used.
            SPIDevice(std::mutex& guard,
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

            virtual ~SPIDevice();

            /// get guard
            /// \return Returns the mutex
            [[nodiscard]] std::mutex& get_guard() const
            {
                return guard;
            }

        protected:
            /// Initialize the SPI Device
            /// \param host The host being used for the SPI Device, VSPI or HSPI
            /// \param chip_select The chip select pin or -1 if not used
            /// \return Returns true on success, false on failure
            bool initialize(spi_host_device_t host, gpio_num_t chip_select);

            /// Write
            /// \param transaction The transaction to write to the SPI device
            /// \return Returns true on success, false on failure
            bool write(spi_transaction_t& transaction);

            /// Polling write
            /// \param transaction The transaction to write to SPI Device.  Will return
            ///  when write has be completed.
            /// \return Returns true on success, false on failure
            bool polling_write(spi_transaction_t& transaction);

            /// Queue transactions
            /// \param transaction The transaction to be place in the queue
            /// \return Returns true on success, false on failure
            bool queue_transaction(spi_transaction_t& transaction);

            /// Wait for transaction to finish
            /// Waits for the queued transaction to finish
            /// \return Returns true on success, false on failure
            bool wait_for_transaction_to_finish();

            /// Override this method to perform pre-transmission actions.
            /// @warning This method is runs in ISR context
            virtual void pre_transmission_action()
            {
            }

            /// Override this method to perform post-transmission actions.
            /// @warning This method is runs in ISR context
            virtual void post_transmission_action()
            {
            }

        private:
            /// Pre Transmission Callback
            /// C style callback required by ESP32 SPI driver
            /// \param transaction The current transaction
            static void pre_transmission_callback(spi_transaction_t* trans);

            /// Post Transmission Callback
            /// C style callback required by ESP32 SPI driver
            /// \param transaction The current transaction
            static void post_transmission_callback(spi_transaction_t* trans);

            spi_device_interface_config_t config{};
            spi_device_handle_t dev = nullptr;
            std::mutex& guard;

            /// Convert time to ticks
            /// \param ms Time
            /// \return Ticks
            [[nodiscard]] inline TickType_t to_tick(std::chrono::milliseconds ms) const
            {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

                return pdMS_TO_TICKS(ms.count());
#pragma GCC diagnostic pop
            }
    };
}
