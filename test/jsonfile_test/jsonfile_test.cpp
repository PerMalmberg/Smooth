#include "jsonfile_test.h"

#include <algorithm>
#include <fstream>
#include <cassert>
#include <unistd.h>
#include <json/cJSON/cJSON.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/json/Value.h>
#include <smooth/core/json/JsonFile.h>
#include <smooth/core/filesystem/SPIFlash.h>

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::json;
using namespace smooth::core::timer;
using namespace smooth::core::ipc;
using namespace smooth::core::filesystem;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace jsonfile_test
{

    App::App()
            : Application(APPLICATION_BASE_PRIO, seconds(1))
    {
    }

    void App::tick()
    {
        SPIFlash flash{"/flash", "app_storage", 10, true};
        assert(flash.mount());

        {
            unlink("/flash/file.jsn");
            JsonFile jf{"/flash/file.jsn"};
            auto& v = jf.value();
            assert(v["Foo"].get_string("").empty());
            v["Foo"] = "Bar";
            assert(jf.save());
        }
        {
            JsonFile jf{"/flash/file.jsn"};
            auto& v = jf.value();
            assert(v["Foo"].get_string("") == "Bar");
            unlink("/flash/file.json");
        }


        flash.unmount();
    }

}