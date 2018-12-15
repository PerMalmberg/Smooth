#pragma once

#include <memory>
#include <smooth/core/Application.h>
#include <smooth/core/filesystem/SDCard.h>

namespace sdcard_test
{
    class App
            : public smooth::core::Application
    {
        public:

            App();

            void init() override;

            void tick() override;

        private:
            std::unique_ptr<smooth::core::filesystem::SDCard> card{};
    };
}