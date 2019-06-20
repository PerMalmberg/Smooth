#pragma once

#include <smooth/core/Application.h>
#include <smooth/core/filesystem/SPIFlash.h>
#include <smooth/core/timer/ElapsedTime.h>

namespace spiflash
{
    class App
            : public smooth::core::Application
    {
    public:

        App();

        void init() override;

        void tick() override;

    private:
        // See partitions.csv for partition table layout and where "app_storage" comes from.
        smooth::core::filesystem::SPIFlash flash{"/our_root", "app_storage", 10, true};
        bool mounted = false;
        smooth::core::timer::ElapsedTime elapsed{};
    };
};

