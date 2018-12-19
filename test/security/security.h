#pragma once

#include <functional>
#include <smooth/core/Application.h>
#include <smooth/application/security/PasswordHash.h>

namespace security
{
    class App
            : public smooth::core::Application
    {
        public:
            App();

            void init() override;

            void tick() override;

        private:
            void time(const std::string& password, size_t ops, size_t mem);
            smooth::application::security::PasswordHash ph{};
    };
}