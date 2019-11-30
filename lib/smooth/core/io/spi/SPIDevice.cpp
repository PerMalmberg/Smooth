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
#include "smooth/core/io/spi/SPIDevice.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;

namespace smooth::core::io::spi
{
    static constexpr const char* log_tag = "SPIDevice";
    static constexpr const std::chrono::milliseconds timeout(1000);

    SPIDevice::SPIDevice(
        std::mutex& guard,
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
            : guard(guard)
    {
        config.command_bits = spi_command_bits;
        config.address_bits = spi_address_bits;
        config.dummy_bits = bits_between_address_and_data_phase;
        config.mode = spi_mode;
        config.duty_cycle_pos = spi_positive_duty_cycle;
        config.cs_ena_pretrans = 0; // Only for half-duplex
        config.cs_ena_posttrans = spi_cs_ena_posttrans;
        config.clock_speed_hz = spi_clock_speed_hz;
        config.flags = spi_device_flags;
        config.queue_size = spi_queue_size;

        if (use_pre_transaction_callback)
        {
            config.pre_cb = &SPIDevice::pre_transmission_callback;
        }

        if (use_post_transaction_callback)
        {
            config.post_cb = &SPIDevice::post_transmission_callback;
        }
    }

    SPIDevice::~SPIDevice()
    {
        if (dev != nullptr)
        {
            // make sure no transactions are in flight
            wait_for_transaction_to_finish();

            // remove device from bus
            spi_bus_remove_device(dev);
        }
    }

    bool SPIDevice::initialize(spi_host_device_t host, gpio_num_t chip_select)
    {
        config.spics_io_num = chip_select;

        auto res = spi_bus_add_device(host, &config, &dev);

        if (res == ESP_ERR_INVALID_ARG)
        {
            Log::error(log_tag, "Invalid parameter");
        }
        else if (res == ESP_ERR_NOT_FOUND)
        {
            Log::error(log_tag, "No free CS slots");
        }
        else if (res == ESP_ERR_NO_MEM)
        {
            Log::error(log_tag, "Out of memory");
        }

        return res == ESP_OK;
    }

    bool SPIDevice::write(spi_transaction_t& transaction)
    {
        // Attach ourselves as the user-data
        transaction.user = this;

        auto res = spi_device_transmit(dev, &transaction);

        if (res != ESP_OK)
        {
            Log::error(log_tag, "write() failed");
        }

        return res == ESP_OK;
    }

    bool SPIDevice::polling_write(spi_transaction_t& transaction)
    {
        // Attach ourselves as the user-data
        transaction.user = this;

        auto res = spi_device_polling_transmit(dev, &transaction);

        if (res != ESP_OK)
        {
            Log::error(log_tag, "polling_write() failed");
        }

        return res == ESP_OK;
    }

    bool SPIDevice::queue_transaction(spi_transaction_t& transaction)
    {
        // Attach ourselves as the user-data
        transaction.user = this;

        auto res = spi_device_queue_trans(dev, &transaction, to_tick(timeout));

        if (res != ESP_OK)
        {
            Log::error(log_tag, "queued_transaction() failed");
        }

        return res == ESP_OK;
    }

    bool SPIDevice::wait_for_transaction_to_finish()
    {
        spi_transaction_t* rtrans;

        auto res = spi_device_get_trans_result(dev, &rtrans, to_tick(timeout));

        if (res != ESP_OK)
        {
            Log::error(log_tag, "wait_for_transaction_to_finish() failed");
        }

        return res == ESP_OK;
    }

    void SPIDevice::pre_transmission_callback(spi_transaction_t* trans)
    {
        auto device = reinterpret_cast<SPIDevice*>(trans->user);

        if (device != nullptr)
        {
            //device->pre_transmission_action(trans);
            device->pre_transmission_action();
        }
    }

    void SPIDevice::post_transmission_callback(spi_transaction_t* trans)
    {
        auto device = reinterpret_cast<SPIDevice*>(trans->user);

        if (device != nullptr)
        {
            //device->post_transmission_action(trans);
            device->post_transmission_action();
        }
    }
}
