//
// Created by permal on 8/18/17.
//

#pragma once

#include <memory>
#include <smooth/core/util/make_unique.h>
#include <smooth/core/ipc/Lock.h>
#include <driver/gpio.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include "SPIDevice.h"
#include "esp_log.h"

namespace smooth
{
    namespace core
    {
        namespace io
        {
            namespace spi
            {
                enum SPI_DMA_Channel
                {
                    DMA_NONE = 0,
                    DMA_1,
                    DMA_2,
                };

                class Master
                {
                    private:
                        static constexpr const char* log_tag = "SPIMaster";

                    public:
                        /// Create a driver for the specified host using the specified I/O pins.
                        /// @note Set unused pins to -1.
                        /// \param host The host, either HSPI_HOST or HSPI_HOST.
                        /// \param mosi Master Output, Slave Input, i.e. data pin for sending data to slave.
                        /// \param miso Master Input, Slave Output, i.e. data pin for receiving data from slave.
                        /// \param clock Cock pin.
                        /// \param data__or_command Pin to signal data or command to slave.
                        Master(spi_host_device_t host,
                               SPI_DMA_Channel dma_channel,
                               gpio_num_t mosi,
                               gpio_num_t miso,
                               gpio_num_t clock,
                               gpio_num_t quadwp_io_num = static_cast<gpio_num_t>(-1),
                               gpio_num_t quadhd_io_num = static_cast<gpio_num_t>(-1)
                        );

                        /// Initialize the SPI host.
                        /// \param host The host, either HSPI_HOST or HSPI_HOST
                        /// \return true on success, false on failure
                        bool initialize();

                        /// Adds a device using the provided parameters.
                        /// \tparam DeviceType The type of device to add, must inherit from SPIDevice
                        /// \tparam Args Parameter argument types
                        /// \param chip_select GPIO pin for chip select, or -1 if not used.
                        /// \param args Additional arguments that should be passed to the device.
                        /// \return A unique pointer to a device, or an empty on failure.
                        template<typename DeviceType, typename ...Args>
                        std::unique_ptr<ISPIDevice> add_device(gpio_num_t chip_select, Args&& ...args)
                        {
                            core::ipc::Lock lock(guard);
                            auto device = core::util::make_unique<DeviceType>(guard, std::forward<Args>(args)...);
                            if (device->initialize(host, chip_select))
                            {
                                ESP_LOGV(log_tag, "Device added");
                            }
                            else
                            {
                                device.reset();
                            }
                            return device;
                        }

                    private:
                        bool initialized = false;
                        std::mutex guard{};
                        spi_host_device_t host;
                        SPI_DMA_Channel dma_channel;
                        spi_bus_config_t bus_config{};
                };
            }
        }
    }
}
