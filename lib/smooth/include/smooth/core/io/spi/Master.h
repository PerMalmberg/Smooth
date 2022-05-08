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

#include <memory>
#include <mutex>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <driver/gpio.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#pragma GCC diagnostic pop

namespace smooth::core::io::spi
{
    enum SPI_DMA_Channel
    {
        DMA_NONE = 0,
        DMA_1,
        DMA_2,
    };

    class Master
    {
        public:
            /// Initialize the SPI host.
            /// @note Set unused pins to GPIO_NUM_NC
            /// \param host The host, either HSPI_HOST or HSPI_HOST.
            /// \param dms_chl The DMA channel to use with this host
            /// \param mosi Master Output, Slave Input, i.e. data pin for sending data to slave.
            /// \param miso Master Input, Slave Output, i.e. data pin for receiving data from slave.
            /// \param clock Clock pin.
            /// \param transfer_size Maximum transfer size, in bytes. Defaults to 4094 if 0.
            /// \param quadwp_io_num GPIO pin for WP (Write Protect) signal used in D2 4-bit communication modes
            /// \param quadhd_io_num GPIO pin for HD (HolD) used in D3 4-bit communication modes
            /// \return true on success, false on failure
            static bool initialize(spi_host_device_t host,
                                   SPI_DMA_Channel dma_chl,
                                   gpio_num_t mosi,
                                   gpio_num_t miso,
                                   gpio_num_t clock,
                                   int transfer_size = 0,
                                   gpio_num_t quadwp_io_num = GPIO_NUM_NC,
                                   gpio_num_t quadhd_io_num = GPIO_NUM_NC);

            /// Deinitialize
            /// Perfoms steps to de-initialize a particular SPI Master
            /// \param spi_host The spi-host that we want to de-initialize.
            static void deinitialize(spi_host_device_t spi_host);

            /// Create a device of the given type.
            /// \tparam DeviceType The type of device to create must inherit from SPIDevice
            /// \tparam Args Parameter argument types
            /// \param args Additional arguments that should be passed to the device.
            /// \return A unique pointer to a device, or an empty on failure.
            template<typename DeviceType, typename... Args>
            static std::unique_ptr<DeviceType> create_device(Args&& ... args);

        private:
            /// Set constructor private so Master object cannot be created
            Master();

            /// Do spi initialization
            /// Performs steps to initialize the SPI Master
            /// \param host The SPI Host to be initialized either VSPI or HSPI
            /// \param initialized Contains the initialized state of a SPI bus
            /// \param initialized_count Contains the number of times a SPI bus has been initialized
            /// \param spi_host_str The name of the SPI bus being initialized
            /// \return Return true if the SPI bus has been successfully initialized, false on failure
            static bool do_intitialization(spi_host_device_t host,
                                           bool& initialized,
                                           uint8_t& initialized_count,
                                           const char* spi_host_str);

            /// Deinitialize
            /// Perfoms steps to de-initialize a particular SPI Master
            /// \param host The SPI Host to be initialized either VSPI or HSPI
            /// \param initialized Contains the initialized state of a SPI bus
            /// \param initialized_count Contains the number of times a SPI bus has been initialized
            /// \param spi_host_str The name of the SPI bus being initialized
            static void do_deinitialize(spi_host_device_t host,
                                        bool& initialized,
                                        uint8_t& initialized_count,
                                        const char* spi_host_str);

            static bool spi2_initialized;
            static bool spi3_initialized;
            static uint8_t spi2_initialized_count;
            static uint8_t spi3_initialized_count;
            static std::mutex guard;
            static spi_bus_config_t bus_config;
            static spi_host_device_t spi_host;
            static SPI_DMA_Channel dma_channel;
    };

    template<typename DeviceType, typename... Args>
    std::unique_ptr<DeviceType> Master::create_device(Args&& ... args)
    {
        std::unique_ptr<DeviceType> dev;

        std::lock_guard<std::mutex> lock(guard);
        dev = std::make_unique<DeviceType>(guard, std::forward<Args>(args)...);

        return dev;
    }
}
