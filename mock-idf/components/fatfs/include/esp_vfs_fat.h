#pragma once

#include <esp_err.h>
#include <driver/gpio.h>
#include <driver/sdmmc_types.h>
#include <driver/sdspi_host.h>
#include <wear_levelling.h>

typedef struct {
    /**
     * If FAT partition can not be mounted, and this parameter is true,
     * create partition table and format the filesystem.
     */
    bool format_if_mount_failed;
    int max_files;                  ///< Max number of open files

    /**
     * If format_if_mount_failed is set, and mount fails, format the card
     * with given allocation unit size. Must be a power of 2, between sector
     * size and 128 * sector size.
     * For SD cards, sector size is always 512 bytes. For wear_levelling,
     * sector size is determined by CONFIG_WL_SECTOR_SIZE option.
     *
     * Using larger allocation unit size will result in higher read/write
     * performance and higher overhead when storing small files.
     *
     * Setting this field to 0 will result in allocation unit set to the
     * sector size.
     */
    std::size_t allocation_unit_size;
} esp_vfs_fat_mount_config_t;

// Compatibility definition
typedef esp_vfs_fat_mount_config_t esp_vfs_fat_sdmmc_mount_config_t;

/**
 * @brief Convenience function to get FAT filesystem on SD card registered in VFS
 *
 * This is an all-in-one function which does the following:
 * - initializes SDMMC driver or SPI driver with configuration in host_config
 * - initializes SD card with configuration in slot_config
 * - mounts FAT partition on SD card using FATFS library, with configuration in mount_config
 * - registers FATFS library with VFS, with prefix given by base_prefix variable
 *
 * This function is intended to make example code more compact.
 * For real world applications, developers should implement the logic of
 * probing SD card, locating and mounting partition, and registering FATFS in VFS,
 * with proper error checking and handling of exceptional conditions.
 *
 * @note Use this API to mount a card through SDSPI is deprecated. Please call
 *       `esp_vfs_fat_sdspi_mount()` instead for that case.
 *
 * @param base_path     path where partition should be registered (e.g. "/sdcard")
 * @param host_config   Pointer to structure describing SDMMC host. When using
 *                      SDMMC peripheral, this structure can be initialized using
 *                      SDMMC_HOST_DEFAULT() macro. When using SPI peripheral,
 *                      this structure can be initialized using SDSPI_HOST_DEFAULT()
 *                      macro.
 * @param slot_config   Pointer to structure with slot configuration.
 *                      For SDMMC peripheral, pass a pointer to sdmmc_slot_config_t
 *                      structure initialized using SDMMC_SLOT_CONFIG_DEFAULT.
 *                      (Deprecated) For SPI peripheral, pass a pointer to sdspi_slot_config_t
 *                      structure initialized using SDSPI_SLOT_CONFIG_DEFAULT().
 * @param mount_config  pointer to structure with extra parameters for mounting FATFS
 * @param[out] out_card  if not NULL, pointer to the card information structure will be returned via this argument
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
 */
esp_err_t esp_vfs_fat_sdmmc_mount(const char* base_path,
                                  const sdmmc_host_t* host_config,
                                  const void* slot_config,
                                  const esp_vfs_fat_mount_config_t* mount_config,
                                  sdmmc_card_t** out_card);

/**
 * @brief Convenience function to get FAT filesystem on SD card registered in VFS
 *
 * This is an all-in-one function which does the following:
 * - initializes an SPI Master device based on the SPI Master driver with configuration in
 *   slot_config, and attach it to an initialized SPI bus.
 * - initializes SD card with configuration in host_config_input
 * - mounts FAT partition on SD card using FATFS library, with configuration in mount_config
 * - registers FATFS library with VFS, with prefix given by base_prefix variable
 *
 * This function is intended to make example code more compact.
 * For real world applications, developers should implement the logic of
 * probing SD card, locating and mounting partition, and registering FATFS in VFS,
 * with proper error checking and handling of exceptional conditions.
 *
 * @note This function try to attach the new SD SPI device to the bus specified in host_config.
 *       Make sure the SPI bus specified in `host_config->slot` have been initialized by
 *       `spi_bus_initialize()` before.
 *
 * @param base_path     path where partition should be registered (e.g. "/sdcard")
 * @param host_config_input Pointer to structure describing SDMMC host. This structure can be
 *                          initialized using SDSPI_HOST_DEFAULT() macro.
 * @param slot_config   Pointer to structure with slot configuration.
 *                      For SPI peripheral, pass a pointer to sdspi_device_config_t
 *                      structure initialized using SDSPI_DEVICE_CONFIG_DEFAULT().
 * @param mount_config  pointer to structure with extra parameters for mounting FATFS
 * @param[out] out_card If not NULL, pointer to the card information structure will be returned via
 *                      this argument. It is suggested to hold this handle and use it to unmount the card later if
 *                      needed. Otherwise it's not suggested to use more than one card at the same time and unmount one
 *                      of them in your application.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
 */
esp_err_t esp_vfs_fat_sdspi_mount(const char* base_path,
                                  const sdmmc_host_t* host_config_input,
                                  const sdspi_device_config_t* slot_config,
                                  const esp_vfs_fat_mount_config_t* mount_config,
                                  sdmmc_card_t** out_card);

/**
 * @brief Unmount an SD card from the FAT filesystem and release resources acquired using
 *        `esp_vfs_fat_sdmmc_mount()` or `esp_vfs_fat_sdspi_mount()`
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the card argument is unregistered
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount hasn't been called
 */
esp_err_t esp_vfs_fat_sdcard_unmount(const char* base_path, sdmmc_card_t* card);

/**
 * @brief Convenience function to initialize FAT filesystem in SPI flash and register it in VFS
 *
 * This is an all-in-one function which does the following:
 *
 * - finds the partition with defined partition_label. Partition label should be
 *   configured in the partition table.
 * - initializes flash wear levelling library on top of the given partition
 * - mounts FAT partition using FATFS library on top of flash wear levelling
 *   library
 * - registers FATFS library with VFS, with prefix given by base_prefix variable
 *
 * This function is intended to make example code more compact.
 *
 * @param base_path        path where FATFS partition should be mounted (e.g. "/spiflash")
 * @param partition_label  label of the partition which should be used
 * @param mount_config     pointer to structure with extra parameters for mounting FATFS
 * @param[out] wl_handle   wear levelling driver handle
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_FOUND if the partition table does not contain FATFS partition with given label
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_spiflash_mount was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes from wear levelling library, SPI flash driver, or FATFS drivers
 */
esp_err_t esp_vfs_fat_spiflash_mount(const char* base_path,
                                     const char* partition_label,
                                     const esp_vfs_fat_mount_config_t* mount_config,
                                     wl_handle_t* wl_handle);

/**
 * @brief Unmount FAT filesystem and release resources acquired using esp_vfs_fat_spiflash_mount
 *
 * @param base_path  path where partition should be registered (e.g. "/spiflash")
 * @param wl_handle  wear levelling driver handle returned by esp_vfs_fat_spiflash_mount
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_spiflash_mount hasn't been called
 */
esp_err_t esp_vfs_fat_spiflash_unmount(const char* base_path, wl_handle_t wl_handle);
