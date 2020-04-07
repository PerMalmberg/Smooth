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

#include "smooth/core/Application.h"
#include "smooth/core/filesystem/SPIFlash.h"
#include "smooth/core/timer/ElapsedTime.h"

namespace hw_spiflash
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
            smooth::core::filesystem::SPIFlash flash{ smooth::core::filesystem::FlashMount::instance(),
                                                      "app_storage",
                                                      10,
                                               true };
            bool mounted = false;
            smooth::core::timer::ElapsedTime elapsed{};
    };
}
