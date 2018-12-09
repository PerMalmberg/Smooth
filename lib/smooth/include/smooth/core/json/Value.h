#pragma once

#include <memory>
#include <string>
#include <vector>
#include <json/cJSON/cJSON.h>

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
                    explicit Value(cJSON* src, bool owning = false);
                    explicit Value(const std::string& src);
                    Value(cJSON* parent, cJSON* object);

                    Value(const Value& other);
                    Value& operator=(const Value& other);

                    ~Value()
                    {
                        if(owns_data && data)
                        {
                            cJSON_Delete(data);
                        }
                    }

                    // Object accessor
                    Value operator[](const std::string& key);
                    // Array accessor
                    Value operator[](size_t index);

                    Value& operator=(const std::string& s);
                    Value& operator=(int value);
                    Value& operator=(double value);
                    Value& set(bool value);

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
    }
}