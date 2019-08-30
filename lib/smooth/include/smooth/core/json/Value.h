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

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cJSON.h>

namespace smooth::core::json
{
    class Value
    {
        public:
            Value();

            explicit Value(cJSON* src, bool owning = false);

            explicit Value(const std::string& src);

            Value(cJSON* parent, cJSON* object);

            // Note: The assignment operator and copy constructor both
            // decouples the from the original JSON structure, creating a new
            // independent memory structure.
            Value(const Value& other);

            Value& operator=(const Value& other);

            ~Value()
            {
                if (owns_data && data)
                {
                    cJSON_Delete(data);
                }
            }

            // Object accessor
            Value operator[](const std::string& key);

            // Array accessor
            Value operator[](int index);

            Value& operator=(const std::string& s)
            {
                operator=(s.c_str());

                return *this;
            }

            Value& operator=(const char* s);

            Value& operator=(int value);

            Value& operator=(bool value);

            Value& operator=(double value);

            Value& operator=(uint32_t value);

            bool operator==(const std::string& s) const;

            bool operator!=(const std::string& s) const
            {
                return !(*this == s);
            }

            bool operator==(int value) const;

            bool operator!=(int value) const
            {
                return !(*this == value);
            }

            bool operator==(double value) const;

            bool operator!=(double value) const
            {
                return !(*this == value);
            }

            explicit operator const char*() const;

            explicit operator int() const;

            explicit operator double() const;

            explicit operator bool() const;

            std::string get_string(const std::string& default_value_value) const;

            int get_int(int default_value) const;

            bool get_bool(bool default_value) const;

            int get_array_size() const;

            std::string get_name() const;

            void get_member_names(std::vector<std::string>& names) const;

            std::string to_string() const;

            // Deletes named object
            void erase(const std::string& name);

            // Delete item at index in array
            void erase(int index);

        private:
            cJSON* parent = nullptr;
            cJSON* data = nullptr;
            bool owns_data = false;
    };
}
