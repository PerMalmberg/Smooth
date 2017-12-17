//
// Created by permal on 12/17/17.
//

#include <smooth/core/filesystem/Filesystem.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        namespace filesystem
        {

            Filesystem::Filesystem(const char* root, const char* partition_name, int max_file_count,
                                   bool format_on_mount_failure)
                    : root(root),
                      partition_name(partition_name),
                      max_file_count(max_file_count),
                      format_on_mount_failure(format_on_mount_failure)
            {
            }

            bool Filesystem::mount()
            {
                esp_vfs_fat_mount_config_t mount_config
                        {
                                format_on_mount_failure,
                                max_file_count
                        };

                {
                    Format msg("Mounting partition {1} in root {2}, with max file count {3}...",
                               Str(partition_name),
                               Str(root),
                               Int32(max_file_count));
                    Log::info(tag, msg);
                }
                auto res = esp_vfs_fat_spiflash_mount(root.c_str(),
                                                      partition_name.c_str(),
                                                      &mount_config,
                                                      &wl_instance);

                Format msg("Result: {1}", Str(res == ESP_OK ? "OK" : "FAIL"));

                if (res == ESP_OK)
                {
                    Log::info(tag, msg);
                }
                else
                {
                    Log::error(tag, msg);
                }

                return res == ESP_OK;
            }

            void Filesystem::unmount()
            {
                if (wl_instance != WL_INVALID_HANDLE)
                {
                    {
                        Format msg("Unmounting partition {1} in root {2}.",
                                   Str(partition_name),
                                   Str(root));
                        Log::info(tag, msg);
                    }

                    auto res = esp_vfs_fat_spiflash_unmount(root.c_str(), wl_instance);

                    Format msg("Result: {1}", Str(res == ESP_OK ? "OK" : "FAIL"));

                    if (res == ESP_OK)
                    {
                        Log::info(tag, msg);
                    }
                    else
                    {
                        Log::error(tag, msg);
                    }
                }
            }
        }
    }
}