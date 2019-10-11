#pragma once

#include <esp_err.h>
#include <wear_leveling.h>
#include <sdmmc_cmd.h>

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

esp_err_t esp_vfs_fat_spiflash_mount(const char* base_path,
                                     const char* partition_label,
                                     const esp_vfs_fat_mount_config_t* mount_config,
                                     wl_handle_t* wl_handle);

esp_err_t esp_vfs_fat_spiflash_unmount(const char* base_path, wl_handle_t wl_handle);

esp_err_t esp_vfs_fat_sdmmc_mount(const char* base_path,
                                  const sdmmc_host_t* host_config,
                                  const void* slot_config,
                                  const esp_vfs_fat_mount_config_t* mount_config,
                                  sdmmc_card_t** out_card);

esp_err_t esp_vfs_fat_sdmmc_unmount();

esp_err_t esp_vfs_fat_spiflash_mount(const char* base_path,
                                     const char* partition_label,
                                     const esp_vfs_fat_mount_config_t* mount_config,
                                     wl_handle_t* wl_handle);

esp_err_t esp_vfs_fat_spiflash_unmount(const char* base_path, wl_handle_t wl_handle);
