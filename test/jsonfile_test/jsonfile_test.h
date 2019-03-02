#pragma once

#include <smooth/core/Application.h>

namespace jsonfile_test
{
    class App
            : public smooth::core::Application
    {
    public:
        App();

        void tick() override;

    private:
    };
}