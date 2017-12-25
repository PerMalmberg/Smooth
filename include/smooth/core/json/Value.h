#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cJSON.h>

namespace smooth
{
    namespace core
    {
        namespace json
        {
            class Value
            {
                public:
                    Value();
                    explicit Value(cJSON* src);
                    explicit Value(const std::string& src);
                    Value(cJSON* parent, cJSON* object);

                    ~Value()
                    {
                        if(owned_data)
                        {
                            cJSON_Delete(owned_data);
                        }
                    }

                    // Object accessor
                    Value operator[](const std::string& key);
                    // Array accessor
                    Value operator[](int index);

                    Value& operator=(const std::string& s);
                    Value& operator=(int value);
                    Value& operator=(double value);
                    Value& set(bool value);
                    Value& operator=(const Value& other);

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
                private:
                    cJSON* parent = nullptr;
                    cJSON* data = nullptr;
                    cJSON* owned_data = nullptr;
            };
        }
    }
}