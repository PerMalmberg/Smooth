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

#include "hw_jsonfile_test.h"

#include <algorithm>
#include <fstream>
#include <cassert>
#include <unistd.h>
#include "smooth/core/task_priorities.h"
#include "smooth/core/json/JsonFile.h"
#include "smooth/core/filesystem/SPIFlash.h"

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::json;
using namespace smooth::core::timer;
using namespace smooth::core::ipc;
using namespace smooth::core::filesystem;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace hw_jsonfile_test
{
    App::App()
            : Application(APPLICATION_BASE_PRIO, seconds(1))
    {
    }

    void App::tick()
    {
        SPIFlash flash{ FlashMount::instance(), "app_storage", 10, true };
        assert(flash.mount());

        {
            unlink(FlashMount::instance().mount_point() / "file.jsn");
            JsonFile jf{ FlashMount::instance().mount_point() / "file.jsn" };
            auto& v = jf.value();
            assert(v["Foo"].is_null());
            v["Foo"] = "Bar";
            assert(jf.save());
        }
        {
            JsonFile jf{ FlashMount::instance().mount_point() / "file.jsn" };
            auto& v = jf.value();
            assert(v["Foo"].get<std::string>() == "Bar");
            unlink(FlashMount::instance().mount_point() / "file.json");
        }

        flash.unmount();
    }
}
