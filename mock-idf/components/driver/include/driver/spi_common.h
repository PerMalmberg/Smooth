// Copyright 2010-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "hal/spi_types.h"

/**
 * @brief This is a configuration structure for a SPI bus.
 *
 * You can use this structure to specify the GPIO pins of the bus. Normally, the driver will use the
 * GPIO matrix to route the signals. An exception is made when all signals either can be routed through
 * the IO_MUX or are -1. In that case, the IO_MUX is used, allowing for >40MHz speeds.
 *
 * @note Be advised that the slave driver does not use the quadwp/quadhd lines and fields in spi_bus_config_t refering to these lines will be ignored and can thus safely be left uninitialized.
 */
typedef struct {
    int mosi_io_num;                ///< GPIO pin for Master Out Slave In (=spi_d) signal, or -1 if not used.
    int miso_io_num;                ///< GPIO pin for Master In Slave Out (=spi_q) signal, or -1 if not used.
    int sclk_io_num;                ///< GPIO pin for Spi CLocK signal, or -1 if not used.
    int quadwp_io_num;              ///< GPIO pin for WP (Write Protect) signal which is used as D2 in 4-bit
                                    // communication modes, or -1 if not used.
    int quadhd_io_num;              ///< GPIO pin for HD (HolD) signal which is used as D3 in 4-bit communication modes,
                                    // or -1 if not used.
    int max_transfer_sz;            ///< Maximum transfer size, in bytes. Defaults to 4094 if 0.
    uint32_t flags;                 ///< Abilities of bus to be checked by the driver. Or-ed value of
                                    // ``SPICOMMON_BUSFLAG_*`` flags.
    int intr_flags;    /**< Interrupt flag for the bus to set the priority, and IRAM attribute, see
                         *  ``esp_intr_alloc.h``. Note that the EDGE, INTRDISABLED attribute are ignored
                         *  by the driver. Note that if ESP_INTR_FLAG_IRAM is set, ALL the callbacks of
                         *  the driver, and their callee functions, should be put in the IRAM.
                         */
} spi_bus_config_t;

/**
 * @brief Initialize a SPI bus
 *
 * @warning For now, only supports HSPI and VSPI.
 *
 * @param host SPI peripheral that controls this bus
 * @param bus_config Pointer to a spi_bus_config_t struct specifying how the host should be initialized
 * @param dma_chan Either channel 1 or 2, or 0 in the case when no DMA is required. Selecting a DMA channel
 *                 for a SPI bus allows transfers on the bus to have sizes only limited by the amount of
 *                 internal memory. Selecting no DMA channel (by passing the value 0) limits the amount of
 *                 bytes transfered to a maximum of 64. Set to 0 if only the SPI flash uses
 *                 this bus.
 *
 * @warning If a DMA channel is selected, any transmit and receive buffer used should be allocated in
 *          DMA-capable memory.
 *
 * @warning The ISR of SPI is always executed on the core which calls this
 *          function. Never starve the ISR on this core or the SPI transactions will not
 *          be handled.
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   if configuration is invalid
 *         - ESP_ERR_INVALID_STATE if host already is in use
 *         - ESP_ERR_NO_MEM        if out of memory
 *         - ESP_OK                on success
 */
esp_err_t spi_bus_initialize(spi_host_device_t host, const spi_bus_config_t* bus_config, int dma_chan);

/**
 * @brief Free a SPI bus
 *
 * @warning In order for this to succeed, all devices have to be removed first.
 *
 * @param host SPI peripheral to free
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_INVALID_STATE if not all devices on the bus are freed
 *         - ESP_OK                on success
 */
esp_err_t spi_bus_free(spi_host_device_t host);
