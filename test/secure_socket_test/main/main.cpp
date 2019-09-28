#include <smooth/core/SystemStatistics.h>
#include "secure_socket_test.h"

using namespace secure_socket_test;

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