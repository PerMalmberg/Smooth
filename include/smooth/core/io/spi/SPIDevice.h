//
// Created by permal on 8/18/17.
//

#pragma once

#include <functional>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include <smooth/core/ipc/Mutex.h>
#include "ISPIDevice.h"

namespace smooth
{
    namespace core
    {
        namespace io
        {
            namespace spi
            {
                class SPIDevice
                        : public ISPIDevice
                {
                    public:
                        /// Base constructor for SPI devices.
                        /// @note These arguments corresponds to those in the spi_device_interface_config_t structure.
                        /// \param The guard mutex (owned by the SPI Master) used to synchronize SPI transmisions between devices.
                        /// \param command_bits Number of bits in command phase (0-16)
                        /// \param address_bits Number of bits in address phase (0-64)
                        /// \param bits_between_address_and_data_phase Number of bits to insert between address and data phase
                        /// \param spi_mode SPI mode (0-3)
                        /// \param positive_duty_cycle Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). Setting this to 0 (=not setting it) is equivalent to setting this to 128.
                        /// \param cs_ena_posttrans Amount of SPI bit-cycles the cs should stay active after the transmission (0-16)
                        /// \param clock_speed_hz Clock speed, in Hz
                        /// \param flags Bitwise OR of SPI_DEVICE_* flags
                        /// \param queue_size Transaction queue size. This sets how many transactions can be 'in the air' (queued using spi_device_queue_trans but not yet finished using spi_device_get_trans_result) at the same time
                        SPIDevice(core::ipc::Mutex& guard,
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
                                  bool use_post_transaction_callback);

                        ~SPIDevice();
                        bool initialize(spi_host_device_t host, gpio_num_t chip_select);

                        bool write(spi_transaction_t& transaction) override;

                        virtual core::ipc::Mutex& get_guard() const
                        {
                            return guard;
                        }

                    protected:
                        /// Override this method to perform pre-transmission actions.
                        /// @warning This method is run in ISR context
                        /// \param trans The transmission data
                        virtual void pre_transmission_action(spi_transaction_t* trans)
                        {
                        }

                        /// Override this method to perform post-transmission actions.
                        /// @warning This method is run in ISR context
                        /// \param trans The transmission data
                        virtual void post_transmission_action(spi_transaction_t* trans)
                        {
                        }

                    private:
                        core::ipc::Mutex& guard;
                        static void pre_transmission_callback(spi_transaction_t* trans);
                        static void post_transmission_callback(spi_transaction_t* trans);
                        spi_device_interface_config_t config{};
                        spi_device_handle_t dev = nullptr;
                };
            }
        }
    }
}