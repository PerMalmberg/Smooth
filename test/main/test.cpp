//
// Created by permal on 2018-10-21.
//

#include "test.h"

#include <hello_world.h>

extern "C"
{
#ifdef ESP_PLATFORM
void app_main()
{
    hello_world::App app;
    app.start();
}
#else
int main(int argc, char** argv)
{
    hello_world::App app;
    app.start();
    return 0;
}
#endif

}