// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

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