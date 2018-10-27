#pragma once

#include <sdmmc_cmd.h>
#include <vfs/include/esp_vfs.h>

namespace smooth
{
    namespace core
    {
        namespace filesystem
        {
            class SDCard
            {
                public:
                    virtual bool init(const char *mount_point, bool format_on_mount_failure, int max_file_count) = 0;

                    virtual bool is_initialized() const {return initialized;}

                protected:
                    sdmmc_host_t host{};
                    sdmmc_card_t* card;
                    bool initialized{};
                private:
            };


        }
    }
}