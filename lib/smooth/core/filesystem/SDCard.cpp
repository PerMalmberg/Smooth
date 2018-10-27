
#include <smooth/core/logging/log.h>
#include <smooth/core/filesystem/SDCard.h>


using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        namespace filesystem
        {

            bool SDCard::do_common_initialization(const char* mount_point, int max_file_count, bool format_on_mount_failure, void* slot_config)
            {
                esp_vfs_fat_sdmmc_mount_config_t mount_config{};
                mount_config.format_if_mount_failed = format_on_mount_failure;
                mount_config.max_files = max_file_count;
                //mount_config.allocation_unit_size = 16 * 1024;

                auto mount_result = esp_vfs_fat_sdmmc_mount(mount_point, &host, slot_config, &mount_config, &card);

                initialized = mount_result == ESP_OK;
                if (initialized)
                {
                    Log::info("SPISDCard", Format("SD Card initialized"));
                    sdmmc_card_print_info(stdout, card);
                }
                else
                {
                    if (mount_result == ESP_FAIL)
                    {
                        Log::error("SPISDCard", Format("Failed to mount the file system."));
                    }
                    else
                    {
                        Log::error("SPISDCard", Format( "Failed to initialize SD Card." ));
                    }
                }

                return initialized;
            }
        }
    }
}