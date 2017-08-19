//
// Created by permal on 8/18/17.
//

#include <smooth/core/io/spi/SPIDevice.h>
#include "esp_log.h"

namespace smooth
{
    namespace core
    {
        namespace io
        {
            namespace spi
            {
                static const char* log_tag = "SPIDevice";

                SPIDevice::SPIDevice(
                        core::ipc::Mutex& guard,
                        uint8_t command_bits,
                        uint8_t address_bits,
                        uint8_t bits_between_address_and_data_phase,
                        uint8_t spi_mode,
                        uint8_t positive_duty_cycle,
                        uint8_t cs_ena_posttrans,
                        int clock_speed_hz,
                        uint32_t flags,
                        int queue_size,
                        bool use_pre_transaction_callback,
                        bool use_post_transaction_callback)
                        : guard(guard)
                {
                    config.command_bits = command_bits;
                    config.address_bits = address_bits;
                    config.dummy_bits = bits_between_address_and_data_phase;
                    config.mode = spi_mode;
                    config.duty_cycle_pos = positive_duty_cycle;
                    config.cs_ena_pretrans = 0; // Only for half-duplex
                    config.cs_ena_posttrans = cs_ena_posttrans;
                    config.clock_speed_hz = clock_speed_hz;
                    config.flags = flags;
                    config.queue_size = queue_size;

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
                        spi_bus_remove_device(dev);
                    }
                }

                bool SPIDevice::initialize(spi_host_device_t host, gpio_num_t chip_select)
                {
                    config.spics_io_num = chip_select;

                    auto res = spi_bus_add_device(host, &config, &dev);

                    if (res == ESP_ERR_INVALID_ARG)
                    {
                        ESP_LOGE(log_tag, "Invalid parameter");
                    }
                    else if (res == ESP_ERR_NOT_FOUND)
                    {
                        ESP_LOGE(log_tag, "No free CS slots");
                    }
                    else if (res == ESP_ERR_NO_MEM)
                    {
                        ESP_LOGE(log_tag, "Out of memory");
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
                        ESP_LOGE(log_tag, "write() failed");
                    }

                    return res == ESP_OK;
                }

                void SPIDevice::pre_transmission_callback(spi_transaction_t* trans)
                {
                    auto device = reinterpret_cast<SPIDevice*>(trans->user);
                    if (device != nullptr)
                    {
                        device->pre_transmission_action(trans);
                    }
                }

                void SPIDevice::post_transmission_callback(spi_transaction_t* trans)
                {
                    auto device = reinterpret_cast<SPIDevice*>(trans->user);
                    if (device != nullptr)
                    {
                        device->post_transmission_action(trans);
                    }
                }
            }
        }
    }
}