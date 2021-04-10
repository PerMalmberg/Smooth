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

#include <vector>
#include <mutex>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <driver/i2c.h>
#pragma GCC diagnostic pop
#include <driver/gpio.h>
#include "smooth/core/util/FixedBuffer.h"
#include "smooth/core/io/i2c/I2CCommandLink.h"

namespace smooth::core::io::i2c
{
    /// Base class for all I2C master devices.
    class I2CMasterDevice
    {
        public:
            /// Constructor
            /// \param port The port
            /// \param address The device address
            /// \param guard The guard mutex.
            I2CMasterDevice(i2c_port_t port, uint8_t address, std::mutex& guard)
                    : address(address), port(port), guard(guard)
            {
            }

            virtual ~I2CMasterDevice() = default;

            /// Scans the bus for devices and reports each found device's address in the provided vector.
            /// \param found_devices Where the address of found devices are placed
            void scan_i2c_bus(std::vector<uint8_t>& found_devices) const;

            [[nodiscard]] std::mutex& get_guard() const
            {
                return guard;
            }

            /// Determines if a device with the given address is present on the bus.
            /// \return true if present, otherwise false.
            [[nodiscard]] bool is_present() const;

            /// Gets the address of the device
            /// \return The address
            [[nodiscard]] uint8_t get_address() const
            {
                return address;
            }

        protected:
            /// Writes the data in the vector to the slave with the provided address.
            /// Usually the data consists of one or more pairs of register and data bytes.
            /// \param address The slave address.
            /// \param data The data to write
            /// \param expect_ack if true, expect ACK from slave.
            /// \return true on success, false on failure
            bool write(uint8_t address, std::vector<uint8_t>& data, bool expect_ack = true);

            /// Reads data from the 8 bit register of the slave with the provided address.  This function DOES NOT
            /// support slave clock-stretching
            /// \param address The slave address
            /// \param slave_register The register to read from.
            /// \param dest Where the data will be written to. The size of the buffer determines how many bytes to read.
            /// \param use_restart_signal If true, uses a start-condition instead of a stop-condition after the slave
            /// address.
            /// \param end_with_nack If true, ends the transmission with a NACK instead of an ACK.
            /// \return true on success, false on failure.
            bool read(uint8_t address,
                      uint8_t slave_register,
                      core::util::FixedBufferBase<uint8_t>& dest,
                      bool use_restart_signal = true,
                      bool end_with_nack = true);

            /// Reads data from the 8 bit register of the slave with the provided address. This function supports slave
            /// clock-stretching
            /// \param address The slave address
            /// \param slave_register The 16 bit register to read from.
            /// \param dest Where the data will be written to. The size of the buffer determines how many bytes to read.
            /// \param use_restart_signal If true, uses a start-condition instead of a stop-condition after the slave
            /// address.
            /// \param end_with_nack If true, ends the transmission with a NACK instead of an ACK.
            /// \param scl_timeout Used to suppport slave clock-stretching.  Slave must release SCL line within this
            /// time.  Set to 0 to disable using slave clock-stretching. The maximum timeout value is 0xFFFFF
            /// (1,048,575)
            /// or 13ms.  Negative numbers will be ignored.
            /// \return true on success, false on failure.
            bool read8(uint8_t address,
                       uint8_t slave_register,
                       core::util::FixedBufferBase<uint8_t>& dest,
                       bool use_restart_signal = true,
                       bool end_with_nack = true,
                       int scl_timeout = 0);

            /// Reads data from the 16 bit register of the slave with the provided address. This function supports slave
            /// clock-stretching
            /// \param address The slave address
            /// \param slave_register The 16 bit register to read from.
            /// \param dest Where the data will be written to. The size of the buffer determines how many bytes to read.
            /// \param use_restart_signal If true, uses a start-condition instead of a stop-condition after the slave
            /// address.
            /// \param end_with_nack If true, ends the transmission with a NACK instead of an ACK.
            /// \param scl_timeout Used to suppport slave clock-stretching.  Slave must release SCL line within this
            /// time.  Set to 0 to disable using slave clock-stretching.  The maximum timeout value is 0xFFFFF
            /// (1,048,575) or 13ms.  Negative numbers will be ignored.
            /// \return true on success, false on failure.
            bool read16(uint8_t address,
                        uint16_t slave_register,
                        core::util::FixedBufferBase<uint8_t>& dest,
                        bool use_restart_signal = true,
                        bool end_with_nack = true,
                        int scl_timeout = 0);

            /// Reads a block of data from the slave with the provided address.  This function DOES NOT support slave
            /// clock-stretching
            /// \param address The slave address
            /// \param dest Where the data will be written to. The size of the buffer determines how many bytes to read.
            /// \param end_with_nack If true, ends the transmission with a NACK instead of an ACK.
            /// \return true on success, false on failure.
            bool read_block(uint8_t address,
                            core::util::FixedBufferBase<uint8_t>& dest,
                            bool end_with_nack = true);

            uint8_t address;
        protected:
            i2c_port_t port;
        private:
            /// Write followed by read
            /// \param link The I2C link command
            /// \param address The slave address
            /// \param slave_reg A FixedBuffer that hold the slave register address
            /// \param dest Where the data will be written to. The size of the buffer determines how many bytes to read.
            /// \param use_restart_signal If true, uses a start-condition instead of a stop-condition after the slave
            /// address.
            /// \param end_with_nack If true, ends the transmission with a NACK instead of an ACK.
            /// \param scl_timeout Used to suppport slave clock-stretching.  Slave must release SCL line within this
            /// time.  Set to 0 to disable using slave clock-stretching.  The maximum timeout value is 0xFFFFF
            /// (1,048,575) or 13ms.  Negative numbers will be ignored.
            /// \return true on success, false on failure.
            bool write_followed_by_read(I2CCommandLink& link,
                                        uint8_t address,
                                        core::util::FixedBufferBase<uint8_t>& slave_reg,
                                        core::util::FixedBufferBase<uint8_t>& dest,
                                        bool use_restart_signal = true,
                                        bool end_with_nack = true,
                                        int scl_timeout = 0);

            /// Log error
            /// \param err The error type
            /// \param msg The error message
            void log_error(esp_err_t err, uint8_t address);

            /// Sets the name of the procees that is being executed
            /// \param name The name of the process
            void set_process_name(const char* name);

            /// Do cleanup - performs cleanup tasks upon completion of a command-link
            void do_cleanup();

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

            std::mutex& guard;
            const char* process_name{};
    };
}
