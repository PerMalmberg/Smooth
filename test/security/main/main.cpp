#include <smooth/core/SystemStatistics.h>
#include "security.h"

using namespace security;

extern "C"
{
#ifdef ESP_PLATFORM

void app_main()
{
    App app{};
    app.start();
}

#else
int main(int /*argc*/, char** /*argv*/)
{
    smooth::core::SystemStatistics::instance().dump();
    App app{};
    app.start();

    return 0;
}

#endif
}