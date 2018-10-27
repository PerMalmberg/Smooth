#pragma once

#include <memory>
#include <smooth/core/Application.h>
#include <smooth/core/filesystem/SDCard.h>
#include <smooth/core/io/i2c/Master.h>
#include <smooth/application/io/MCP23017.h>

namespace sdcard_spi
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