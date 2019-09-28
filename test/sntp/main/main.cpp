#include <smooth/core/SystemStatistics.h>
#include "sntp.h"

using namespace sntp;

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