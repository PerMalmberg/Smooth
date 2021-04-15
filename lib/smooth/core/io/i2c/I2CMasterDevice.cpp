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

#include <algorithm>
#include <sstream>
#include "smooth/core/Task.h"
#include "smooth/core/io/i2c/I2CMasterDevice.h"
#include "smooth/core/io/i2c/I2CCommandLink.h"
#include "smooth/core/logging/log.h"
#include "esp_intr_alloc.h"

using namespace smooth::core::logging;
using namespace smooth::core::util;

namespace smooth::core::io::i2c
{
    // Understanding I2C: http://www.ti.com/lit/an/slva704/slva704.pdf
    // I2C specification: http://www.nxp.com/docs/en/user-guide/UM10204.pdf
    // Clock Stretching:  https://www.i2c-bus.org/clock-stretching/

    constexpr const char* log_tag = "I2CMasterDevice";
    constexpr const char* write_tag = "write";
    constexpr const char* read_tag = "read";
    constexpr const char* read8_tag = "read8";
    constexpr const char* read16_tag = "read16";
    constexpr const char* read_block_tag = "read_block";
    constexpr const std::chrono::milliseconds timeout(1000);

    // Write address and data
    bool I2CMasterDevice::write(uint8_t address, std::vector<uint8_t>& data, bool expect_ack)
    {
        // Create a threadsafe command link
        I2CCommandLink link(*this);

        // Set the process name that will be used by log_error
        set_process_name(write_tag);

        // Set R/W bit to 0 for write.
        auto write_address = static_cast<uint8_t>(address << 1);

        // Build the command link and execute the command link
        auto res = i2c_master_start(link)
                   | i2c_master_write_byte(link, write_address, expect_ack)
                   | i2c_master_write(link, data.data(), data.size(), expect_ack)
                   | i2c_master_stop(link)
                   | i2c_master_cmd_begin(port, link, to_tick(timeout));

        if (res != ESP_OK)
        {
            log_error(res, address);
            do_cleanup();
        }

        return res == ESP_OK;
    }

    // Write address and 8 bit slave register followed by write read address and read data
    // NOTE: This function does not support slave clock-stretching
    bool I2CMasterDevice::read(uint8_t address,
                               uint8_t slave_register,
                               FixedBufferBase<uint8_t>& data,
                               bool use_restart_signal,
                               bool end_with_nack)
    {
        // Create a threadsafe command link
        I2CCommandLink link(*this);

        // Set the process name that will be used by log_error
        set_process_name(read_tag);

        // Convert slave register to FixedBuffer
        FixedBuffer<uint8_t, 1> slave_reg_addr;
        slave_reg_addr[0] = slave_register;

        return write_followed_by_read(link, address, slave_reg_addr, data, use_restart_signal, end_with_nack, 0);
    }

    // Write address and 8 bit slave register followed by write read address and read data.
    // NOTE: This function supports slave clock-stretching
    bool I2CMasterDevice::read8(uint8_t address,
                                uint8_t slave_register,
                                FixedBufferBase<uint8_t>& data,
                                bool use_restart_signal,
                                bool end_with_nack,
                                int scl_timeout)
    {
        // Create a threadsafe command link
        I2CCommandLink link(*this);

        // Set the process name that will be used by log_error
        set_process_name(read8_tag);

        // Convert slave register to FixedBuffer
        FixedBuffer<uint8_t, 1> slave_reg;
        slave_reg[0] = slave_register;

        return write_followed_by_read(link, address, slave_reg, data, use_restart_signal, end_with_nack, scl_timeout);
    }

    // Write address and 16 bit slave register followed by write read address and read data.
    // NOTE: This function supports slave clock-stretching
    bool I2CMasterDevice::read16(uint8_t address,
                                 uint16_t slave_register,
                                 FixedBufferBase<uint8_t>& data,
                                 bool use_restart_signal,
                                 bool end_with_nack,
                                 int scl_timeout)
    {
        // Create a threadsafe command link
        I2CCommandLink link(*this);

        // Set the process name that will be used by log_error
        set_process_name(read16_tag);

        // Convert slave register to FixedBuffer
        FixedBuffer<uint8_t, 2> slave_reg;
        slave_reg[0] = static_cast<uint8_t>(slave_register >> 8);
        slave_reg[1] = static_cast<uint8_t>(slave_register & 0x00FF);

        return write_followed_by_read(link, address, slave_reg, data, use_restart_signal, end_with_nack, scl_timeout);
    }

    // Write address and slave register followed by write read address and read data
    // This function supports slave clock-stretching
    bool I2CMasterDevice::write_followed_by_read(I2CCommandLink& link,
                                                 uint8_t address,
                                                 FixedBufferBase<uint8_t>& slave_reg,
                                                 FixedBufferBase<uint8_t>& data,
                                                 bool use_restart_signal,
                                                 bool end_with_nack,
                                                 int scl_timeout)
    {
        int orig_scl_timeout = 0;
        esp_err_t res = ESP_OK;

        // Set R/W bit to 0 for write.
        auto write_address = static_cast<uint8_t>(address << 1);

        // Set R/W bit to 1 for read.
        auto read_address = static_cast<uint8_t>((address << 1) | 0x1);

        // Check to see if we need to implement slave clock-stretching, if so save the current timeout
        // before changing so we can restore before returning from this function.
        if (scl_timeout > 0)
        {
            res = i2c_get_timeout(port, &orig_scl_timeout);
            res |= i2c_set_timeout(port, scl_timeout);

            if (res != ESP_OK)
            {
                log_error(res, address);
            }
        }

        if (res == ESP_OK)
        {
            // Generate start condition, Write the slave address to slave, Write slave register address to slave
            res = i2c_master_start(link);
            res |= i2c_master_write_byte(link, write_address, true);
            res |= i2c_master_write(link, slave_reg.data(), slave_reg.size(), true);

            // Generate another start condition or stop condition
            if (use_restart_signal)
            {
                res |= i2c_master_start(link);
            }
            else
            {
                // Finish the transmission without releasing the lock we have on the i2c master.
                res |= i2c_master_stop(link);
                res |= i2c_master_cmd_begin(port, link, to_tick(timeout));

                // Start a new transmission
                link.reset();
                res |= i2c_master_start(link);
            }

            // Write the read address, then read the desired amount,
            // ending the read with a NACK to signal the slave to stop sending data.
            res |= i2c_master_write_byte(link, read_address, true);

            if (data.size() > 1)
            {
                res |= i2c_master_read(link, data.data(), data.size() - 1, I2C_MASTER_ACK);
            }

            res |= i2c_master_read_byte(link, data.data() + data.size() - 1,
                                        end_with_nack ? I2C_MASTER_NACK : I2C_MASTER_ACK);

            // Complete the read with a stop condition.
            res |= i2c_master_stop(link);

            // Send the queued commands
            res |= i2c_master_cmd_begin(port, link, to_tick(timeout));

            // If we implemented slave clock-stretching then restore master timeout to its original value
            if (scl_timeout > 0)
            {
                res |= i2c_set_timeout(port, orig_scl_timeout);
            }

            if (res != ESP_OK)
            {
                log_error(res, address);
                do_cleanup();
            }
        }

        return res == ESP_OK;
    }

    // Write read address and read data
    bool I2CMasterDevice::read_block(uint8_t address,
                                     FixedBufferBase<uint8_t>& data,
                                     bool end_with_nack)
    {
        // Create a threadsafe command link
        I2CCommandLink link(*this);

        // Set the process name that will be used by log_error
        set_process_name(read_block_tag);

        // Set R/W bit to 1 for read.
        auto read_address = static_cast<uint8_t>((address << 1) | 0x1);

        // Generate start condition
        auto res = i2c_master_start(link);

        // Write the read address, then read the desired amount,
        // ending the read with a NACK to signal the slave to stop sending data.
        res |= i2c_master_write_byte(link, read_address, true);

        if (data.size() > 1)
        {
            res |= i2c_master_read(link, data.data(), data.size() - 1, I2C_MASTER_ACK);
        }

        res |= i2c_master_read_byte(link, data.data() + data.size() - 1,
                                    end_with_nack ? I2C_MASTER_NACK : I2C_MASTER_ACK);

        // Complete the read with a stop condition.
        res |= i2c_master_stop(link);

        // Send the queued commands
        res |= i2c_master_cmd_begin(port, link, to_tick(timeout));

        if (res != ESP_OK)
        {
            log_error(res, address);
            do_cleanup();
        }

        return res == ESP_OK;
    }

    // Is slave device present
    bool I2CMasterDevice::is_present() const
    {
        std::vector<uint8_t> found;
        scan_i2c_bus(found);
        auto dev = std::find(found.begin(), found.end(), address);

        return dev != found.end();
    }

    // Scan the I2C bus
    void I2CMasterDevice::scan_i2c_bus(std::vector<uint8_t>& found_devices) const
    {
        // Write the address of each possible device and see if an ACK is received or not.
        for (uint8_t address = 2; address <= 127; ++address)
        {
            // Create a threadsafe command link
            I2CCommandLink link(*this);

            // Set R/W bit to 1 for read.
            auto read_address = static_cast<uint8_t>(address << 1);

            // Build the command link and execute the command link
            auto res = i2c_master_start(link)
                       | i2c_master_write_byte(link, read_address, true)
                       | i2c_master_stop(link)
                       | i2c_master_cmd_begin(port, link, to_tick(timeout));

            if (res != ESP_OK)
            {
                // No ACK, no device on this address
            }
            else
            {
                found_devices.push_back(address);
            }
        }

        // Cleanup
        i2c_reset_tx_fifo(port);
        i2c_reset_rx_fifo(port);
    }

    // Log the error
    void I2CMasterDevice::log_error(esp_err_t err, uint8_t address)
    {
        std::stringstream ss;
        ss << "Error in - " << process_name << " - slave address: 0x" << std::hex << static_cast<int32_t>(address);

        if (err == ESP_ERR_INVALID_ARG)
        {
            Log::error(log_tag, "{} - Parameter error", ss.str());
        }
        else if (err == ESP_FAIL)
        {
            Log::error(log_tag, "{} - Send command error, no ACK from slave", ss.str());
        }
        else if (err == ESP_ERR_INVALID_STATE)
        {
            Log::error(log_tag, "{} - I2C driver not installed or not in master mode", ss.str());
        }
        else if (err == ESP_ERR_TIMEOUT)
        {
            Log::error(log_tag, "{} - Operation timeout, bus busy", ss.str());
        }
        else if (err != ESP_OK)
        {
            Log::error(log_tag, "{} - unknown error: {}", ss.str(), err);
        }
    }

    // Set process name
    void I2CMasterDevice::set_process_name(const char* name)
    {
        process_name = name;
    }

    // Do cleanup
    void I2CMasterDevice::do_cleanup()
    {
        i2c_reset_tx_fifo(port);
        i2c_reset_rx_fifo(port);
    }
}
