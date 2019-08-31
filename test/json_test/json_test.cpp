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

#include "json_test.h"

#include <algorithm>
#include <fstream>
#include <cassert>
#include <cJSON.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/json/Value.h>
#include "test_example.json.h"

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::json;
using namespace smooth::core::timer;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace json_test
{
    App::App()
            : Application(APPLICATION_BASE_PRIO, seconds(1))
    {
    }

    void App::tick()
    {
        auto* json = cJSON_Parse(json_data);
        Value root{ json };
        assert(root["key_with_string"] == "value");
        root["key_with_string"] = "new_value";
        assert(root["key_with_string"] == "new_value");
        assert(root["key_with_int"] == 1);
        root["key_with_int"] = 5;
        assert(root["key_with_int"] == 5);
        int i = static_cast<int>(root["key_with_int"]);
        assert(i == 5);

        assert(root["key_with_object"]["key_in_object_with_string"] == "the string");
        std::vector<std::string> names{};
        root.get_member_names(names);
        assert(names.size() == 9);
        assert(std::find(names.begin(), names.end(), "io") != names.end());
        assert(std::find(names.begin(), names.end(), "io") != names.end());
        assert(std::find(names.begin(), names.end(), "key_with_false") != names.end());

        assert(root["key_with_object"]["key_in_object_with_string"].get_name() == "key_in_object_with_string");

        auto copy = root["key_with_object"]["key_in_object_with_string"];
        assert(copy == "the string");

        root["key_with_object"]["key_in_object_with_string"] = "another string";
        assert(root["key_with_object"]["key_in_object_with_string"] == "another string");

        std::string str = static_cast<const char*>(root["key_with_object"]["key_in_object_with_string"]);
        assert(str == "another string");

        assert(root["key_with_object"]["key_in_object_with_double"] == 1.2345);
        root["key_with_object"]["key_in_object_with_double"] = 6.789;
        double d = static_cast<double>(root["key_with_object"]["key_in_object_with_double"]);
        assert(d == 6.789);

        assert(!root["key_with_object"]["key_in_object_with_false"].get_bool(true));
        assert(root["key_with_object"]["key_in_object_with_true"].get_bool(false));

        assert(root["io"]["reference_values"]["digital"]["i0"]["ref"].get_bool(false));
        assert(!root["io"]["reference_values"]["digital"]["i1"]["ref"].get_bool(true));

        assert(root["array"].get_array_size() == 6);
        assert(root["array"][2] == "a2");
        assert(root["array"][5]["value"].get_bool(false));
        assert(!root["array"][5]["value2"].get_bool(true));

        root["array"][0] = "asdf";
        assert(root["array"][0] == "asdf");

        root["array"][1] = 1;
        assert(root["array"][1] == 1);

        root["array"][2] = 3.14;
        assert(root["array"][2] == 3.14);

        // Accessing non existing index adds items to the array
        root["array"][6] = "sixth index";
        assert(root["array"][6] == "sixth index");

        root["array"][9] = "ninth index";
        assert(root["array"][9] == "ninth index");

        root["array"][8] = 8;
        assert(root["array"][8] == 8);

        root["array"][7] = "seventh index";
        assert(root["array"][7] == "seventh index");

        // Change object value in array
        root["array"][5]["value2"] = true;
        assert(root["array"][5]["value2"].get_bool(false));

        // Non existing key -> new item of object type so not equal to any string or number.
        // Accessing a non existing key creates it.
        assert(root["non_existing_key"] != "");
        assert(root["non_existing_key"] != 0);
        assert(root["non_existing_key"] != 0.0);

        assert(root["key_with_true"].get_bool(false));
        assert(!root["key_with_false"].get_bool(true));

        auto zones = root["zones"];
        std::vector<std::string> zone_names{};
        zones.get_member_names(zone_names);
        assert(zone_names.size() == 2);
        assert(zones[*zone_names.begin()].get_array_size() == 2);
        assert(zones[*zone_names.rbegin()].get_array_size() == 5);
        assert(zones[*zone_names.begin()][0] == "a0");

        // We can completely change the type of the held object
        root["non_existing_key"] = "asdf";
        assert(root["non_existing_key"] == "asdf");
        root["non_existing_key"] = 456;
        assert(root["non_existing_key"] == 456);
        root["non_existing_key"] = 8.9;
        assert(root["non_existing_key"] == 8.9);

        root["nested1"]["nested2"]["nested3"] = "I'm six feet under";
        assert(root["nested1"]["nested2"]["nested3"] == "I'm six feet under");

        auto print = cJSON_Print(json);
        Log::info("Data", Format("{1}", Str(print)));
        free(print);

        cJSON_Delete(json);

        {
            Value v{};
            v["foo"] = 123;
            assert(v["foo"] == 123);
            v["array"][0] = 0;
            v["array"][1] = 1;
            v["array"][2] = 2;
            assert(v["array"][0] == 0);
            assert(v["array"][1] == 1);
            assert(v["array"][2] == 2);

            Log::info("Data", Format("{1}", Str(v.to_string())));
        }

        {
            Value v{};
            v["bool"] = false;
            assert(!v["bool"].get_bool(true));
            v["bool"] = true;
            assert(v["bool"].get_bool(false));

            v["value"] = "asdf";
            assert(v["value"] == "asdf");
            v.erase("value");
            assert(v["value"].get_string("aaa") == "aaa");
        }

        {
            Value v{};
            v["a"][0] = 0;
            v["a"][1] = 1;
            v["a"][2] = 2;
            assert(v["a"].get_array_size() == 3);
            v["a"].erase(1);
            assert(v["a"].get_array_size() == 2);
            assert(v["a"][0] == 0);
            assert(v["a"][1] == 2);
        }

        {
            Value v = Value{ R"({"a":123})" };
            Log::info("aa", v.to_string().c_str());
            assert(v["a"].get_int(0) == 123);
        }
    }
}
