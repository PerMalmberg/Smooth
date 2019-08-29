#pragma once

#include "esp_err.h"
#include "driver/gpio.h"
#include "sdmmc_types.h"

#define SDSPI_HOST_DEFAULT() {}

typedef struct {
    gpio_num_t gpio_miso;   ///< GPIO number of MISO signal
    gpio_num_t gpio_mosi;   ///< GPIO number of MOSI signal
    gpio_num_t gpio_sck;    ///< GPIO number of SCK signal
    gpio_num_t gpio_cs;     ///< GPIO number of CS signal
    gpio_num_t gpio_cd;     ///< GPIO number of card detect signal
    gpio_num_t gpio_wp;     ///< GPIO number of write protect signal
    gpio_num_t gpio_int;    ///< GPIO number of interrupt line (input) for SDIO card.
    int dma_channel;        ///< DMA channel to be used by SPI driver (1 or 2)
} sdspi_slot_config_t;

#define SDSPI_SLOT_NO_CD    GPIO_NUM_NC ///< indicates that card detect line is not used
#define SDSPI_SLOT_NO_WP    GPIO_NUM_NC ///< indicates that write protect line is not used
#define SDSPI_SLOT_NO_INT   GPIO_NUM_NC ///< indicates that interrupt line is not used

/**
 * Macro defining default configuration of SPI host
 */
#define SDSPI_SLOT_CONFIG_DEFAULT() {\
    .gpio_miso = GPIO_NUM_2, \
    .gpio_mosi = GPIO_NUM_15, \
    .gpio_sck  = GPIO_NUM_14, \
    .gpio_cs   = GPIO_NUM_13, \
    .gpio_cd   = SDSPI_SLOT_NO_CD, \
    .gpio_wp   = SDSPI_SLOT_NO_WP, \
    .gpio_int  = GPIO_NUM_NC, \
    .dma_channel = 1 \
}

/**
 * @brief Initialize SD SPI driver
 *
 * @note This function is not thread safe
 *
 * @return
 *      - ESP_OK on success
 *      - other error codes may be returned in future versions
 */
esp_err_t sdspi_host_init(void);

/**
* @brief Initialize SD SPI driver for the specific SPI controller
*
* @note This function is not thread safe
*
* @note The SDIO over sdspi needs an extra interrupt line. Call ``gpio_install_isr_service()`` before this function.
*
* @param slot         SPI controller to use (HSPI_HOST or VSPI_HOST)
* @param slot_config  pointer to slot configuration structure
*
* @return
*      - ESP_OK on success
*      - ESP_ERR_INVALID_ARG if sdspi_init_slot has invalid arguments
*      - ESP_ERR_NO_MEM if memory can not be allocated
*      - other errors from the underlying spi_master and gpio drivers
*/
esp_err_t sdspi_host_init_slot(int slot, const sdspi_slot_config_t* slot_config);

/**
 * @brief Send command to the card and get response
 *
 * This function returns when command is sent and response is received,
 * or data is transferred, or timeout occurs.
 *
 * @note This function is not thread safe w.r.t. init/deinit functions,
 *       and bus width/clock speed configuration functions. Multiple tasks
 *       can call sdspi_host_do_transaction as long as other sdspi_host_*
 *       functions are not called.
 *
 * @param slot      SPI controller (HSPI_HOST or VSPI_HOST)
 * @param cmdinfo   pointer to structure describing command and data to transfer
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_TIMEOUT if response or data transfer has timed out
 *      - ESP_ERR_INVALID_CRC if response or data transfer CRC check has failed
 *      - ESP_ERR_INVALID_RESPONSE if the card has sent an invalid response
 */
esp_err_t sdspi_host_do_transaction(int slot, sdmmc_command_t *cmdinfo);

/**
 * @brief Set card clock frequency
 *
 * Currently only integer fractions of 40MHz clock can be used.
 * For High Speed cards, 40MHz can be used.
 * For Default Speed cards, 20MHz can be used.
 *
 * @note This function is not thread safe
 *
 * @param slot      SPI controller (HSPI_HOST or VSPI_HOST)
 * @param freq_khz  card clock frequency, in kHz
 * @return
 *      - ESP_OK on success
 *      - other error codes may be returned in the future
 */
esp_err_t sdspi_host_set_card_clk(int slot, uint32_t freq_khz);


/**
 * @brief Release resources allocated using sdspi_host_init
 *
 * @note This function is not thread safe
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if sdspi_host_init function has not been called
 */
esp_err_t sdspi_host_deinit(void);

/**
 * @brief Enable SDIO interrupt.
 *
 * @param slot SPI controller to use (HSPI_HOST or VSPI_HOST)
 *
 * @return
 *      - ESP_OK on success
 */
esp_err_t sdspi_host_io_int_enable(int slot);

/**
 * @brief Wait for SDIO interrupt until timeout.
 *
 * @param slot SPI controller to use (HSPI_HOST or VSPI_HOST)
 * @param timeout_ticks Ticks to wait before timeout.
 *
 * @return
 *      - ESP_OK on success
 */
esp_err_t sdspi_host_io_int_wait(int slot, TickType_t timeout_ticks);
