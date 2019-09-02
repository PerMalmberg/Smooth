// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.

// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include <smooth/core/util/FixedBufferBase.h>

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

            std::mutex& get_guard() const
            {
                return guard;
            }

            /// Determines if a device with the given address is present on the bus.
            /// \return true if present, otherwise false.
            bool is_present() const;

            /// Gets the address of the device
            /// \return The address
            uint8_t get_address() const
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

            /// Reads data from the register of the slave with the provided address.
            /// \param address The slave address
            /// \param slave_register The register to read from.
            /// \param dest Where the data will be written to. The size of the buffer determines how many bytes to read.
            /// \param use_restart_signal If true, uses a start-condition instead of a stop-condition after the slave
            // address.
            /// \param end_with_nack If true, ends the transmission with a NACK instead of an ACK.
            /// \return true on success, false on failure.
            bool read(uint8_t address,
                      uint8_t slave_register,
                      core::util::FixedBufferBase<uint8_t>& dest,
                      bool use_restart_signal = true,
                      bool end_with_nack = true);

            uint8_t address;
        protected:
            i2c_port_t port;
        private:
            void log_error(esp_err_t err, const char* msg);

            /// Convert time to ticks
            /// \param ms Time
            /// \return Ticks
            inline TickType_t to_tick(std::chrono::milliseconds ms) const
            {
                return pdMS_TO_TICKS(ms.count());
            }

            std::mutex& guard;
    };
}
