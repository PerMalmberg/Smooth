#include <esp_vfs_fat.h>

esp_err_t esp_vfs_fat_spiflash_mount(const char* /*base_path*/,
                                     const char* /*partition_label*/,
                                     const esp_vfs_fat_mount_config_t* /*mount_config*/,
                                     wl_handle_t* /*wl_handle*/)
{
    return ESP_OK;
}

esp_err_t esp_vfs_fat_spiflash_unmount(const char* base_path, wl_handle_t wl_handle)
{
    return ESP_OK;
}

esp_err_t esp_vfs_fat_sdmmc_mount(const char* /*base_path*/,
                                  const sdmmc_host_t* /*host_config*/,
                                  const void* /*slot_config*/,
                                  const esp_vfs_fat_mount_config_t* /*mount_config*/,
                                  sdmmc_card_t** /*out_card*/)
{
    return ESP_OK;
}

esp_err_t esp_vfs_fat_sdmmc_unmount()
{
    return ESP_OK;
}
