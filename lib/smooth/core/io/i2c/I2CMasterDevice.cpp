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

namespace smooth::core::io::i2c
{
    // Understanding I2C: http://www.ti.com/lit/an/slva704/slva704.pdf
    // I2C specification: http://www.nxp.com/docs/en/user-guide/UM10204.pdf

    constexpr const char* log_tag = "I2CMasterDevice";
    constexpr const std::chrono::milliseconds timeout(1000);

    bool I2CMasterDevice::write(uint8_t address, std::vector<uint8_t>& data, bool expect_ack)
    {
        I2CCommandLink link(*this);

        // Set R/W bit to 0 for write.
        address = static_cast<uint8_t>(address << 1);

        auto res = i2c_master_start(link);
        res |= i2c_master_write_byte(link, address, expect_ack);
        res |= i2c_master_write(link, data.data(), data.size(), expect_ack);
        res |= i2c_master_stop(link);

        bool write_result = false;

        if (res == ESP_OK)
        {
            res = i2c_master_cmd_begin(port, link, to_tick(timeout));

            if (res == ESP_OK)
            {
                write_result = true;
            }
            else
            {
                std::stringstream ss;
                ss << "Error during write of address 0x" << std::hex << (address >> 1);
                log_error(res, ss.str().c_str());
            }
        }
        else
        {
            log_error(res, "Failed to prepare write");
        }

        if (!write_result)
        {
            i2c_reset_tx_fifo(port);
            i2c_reset_rx_fifo(port);
        }

        return write_result;
    }

    bool I2CMasterDevice::read(uint8_t address, uint8_t slave_register,
                               core::util::FixedBufferBase<uint8_t>& data,
                               bool use_restart_signal,
                               bool end_with_nack)
    {
        I2CCommandLink link(*this);

        // Set R/W bit to 0 for write.
        auto write_address = static_cast<uint8_t>(address << 1);

        // Set R/W bit to 1 for read.
        auto read_address = static_cast<uint8_t>((address << 1) | 0x1);

        // Generate start condition
        auto res = i2c_master_start(link);

        // Write the slave write address followed by the register address.
        res |= i2c_master_write_byte(link, write_address, true);
        res |= i2c_master_write_byte(link, slave_register, true);

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
        res |= i2c_master_cmd_begin(port, link, to_tick(timeout));

        if (res != ESP_OK)
        {
            std::stringstream ss;
            ss << "Error during read of address 0x" << std::hex << static_cast<int32_t>(address);
            log_error(res, ss.str().c_str());
            i2c_reset_tx_fifo(port);
            i2c_reset_rx_fifo(port);
        }

        return res == ESP_OK;
    }

    bool I2CMasterDevice::is_present() const
    {
        std::vector<uint8_t> found;
        scan_i2c_bus(found);
        auto dev = std::find(found.begin(), found.end(), address);

        return dev != found.end();
    }

    void I2CMasterDevice::scan_i2c_bus(std::vector<uint8_t>& found_devices) const
    {
        // Write the address of each possible device and see if an ACK is received or not.
        for (uint8_t address = 2; address <= 127; ++address)
        {
            I2CCommandLink link(*this);
            auto read_address = static_cast<uint8_t>(address << 1);

            auto res = i2c_master_start(link);
            res |= i2c_master_write_byte(link, read_address, true);
            res |= i2c_master_stop(link);
            res |= i2c_master_cmd_begin(port, link, to_tick(timeout));

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

    void I2CMasterDevice::log_error(esp_err_t err, const char* msg)
    {
        if (err == ESP_ERR_INVALID_ARG)
        {
            Log::error(log_tag, "{} - Parameter error", msg);
        }
        else if (err == ESP_FAIL)
        {
            Log::error(log_tag, "{} - Send command error, no ACK from slave", msg);
        }
        else if (err == ESP_ERR_INVALID_STATE)
        {
            Log::error(log_tag, "{} - I2C driver not installed or not in master mode", msg);
        }
        else if (err == ESP_ERR_TIMEOUT)
        {
            Log::error(log_tag, "{} - Operation timeout, bus busy", msg);
        }
        else if (err != ESP_OK)
        {
            Log::error(log_tag, "{} - unknown error: {}", msg, err);
        }
    }
}
