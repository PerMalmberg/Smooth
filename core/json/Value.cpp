#include <smooth/core/json/Value.h>
#include <smooth/core/util/make_unique.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core::util;
using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        namespace json
        {

            Value::Value()
            {
                // We're owning the root object so save it som we can delete it later.
                owned_data = data = cJSON_CreateObject();
            }

            Value::Value(cJSON* src)
            {
                data = src;
            }

            Value::Value(const std::string& src)
            {
                data = cJSON_Parse(src.c_str());
            }

            Value::Value(cJSON* parent, cJSON* object)
                    : parent(parent), data(object)
            {
            }

            Value Value::operator[](const std::string& key)
            {
                cJSON* item = nullptr;

                if (cJSON_IsObject(data))
                {
                    item = cJSON_GetObjectItemCaseSensitive(data, key.c_str());
                }

                if (item)
                {
                    return Value(data, item);
                }
                else
                {
                    auto new_item = cJSON_CreateObject();
                    cJSON_AddItemToObject(data, key.c_str(), new_item);
                    return Value(data, new_item);
                }
            }

            Value Value::operator[](int index)
            {
                if (cJSON_IsArray(data))
                {
                    auto item = cJSON_GetArrayItem(data, index);
                    if (item)
                    {
                        return Value(data, item);
                    }
                    else
                    {
                        return *this;
                    }
                }
                else
                {
                    return *this;
                }
            }

            Value& Value::operator=(const std::string& s)
            {
                auto new_data = cJSON_CreateString(s.c_str());

                if (cJSON_IsArray(parent))
                {
                    cJSON_ReplaceItemViaPointer(parent, data, new_data);
                }
                else if (parent == nullptr)
                {
                    // We're the root, replace it all.
                    cJSON_Delete(data);
                    data = cJSON_CreateString(s.c_str());
                }
                else
                {
                    cJSON_ReplaceItemInObjectCaseSensitive(parent, data->string, new_data);
                    data = new_data;
                }

                return *this;
            }

            Value& Value::operator=(int value)
            {
                auto new_data = cJSON_CreateNumber(value);

                if (cJSON_IsArray(parent))
                {
                    cJSON_ReplaceItemViaPointer(parent, data, new_data);
                }
                else if (parent == nullptr)
                {
                    // We're the root, replace it all.
                    cJSON_Delete(data);
                    data = new_data;
                }
                else
                {
                    cJSON_ReplaceItemInObjectCaseSensitive(parent, data->string, new_data);
                    data = new_data;
                }

                return *this;
            }

            Value& Value::operator=(double value)
            {
                auto new_data = cJSON_CreateNumber(value);

                if (cJSON_IsArray(parent))
                {
                    cJSON_ReplaceItemViaPointer(parent, data, new_data);
                }
                else if (parent == nullptr)
                {
                    // We're the root, replace it all.
                    cJSON_Delete(data);
                    data = new_data;
                }
                else
                {
                    cJSON_ReplaceItemInObjectCaseSensitive(parent, data->string, new_data);
                    data = new_data;
                }

                return *this;
            }

            Value& Value::set(bool value)
            {
                auto new_data = cJSON_CreateBool(value ? cJSON_True : cJSON_False);

                if (cJSON_IsArray(parent))
                {
                    cJSON_ReplaceItemViaPointer(parent, data, new_data);
                }
                else if (parent == nullptr)
                {
                    // We're the root, replace it all.
                    cJSON_Delete(data);
                    data = new_data;
                }
                else
                {
                    cJSON_ReplaceItemInObjectCaseSensitive(parent, data->string, new_data);
                    data = new_data;
                }

                return *this;
            }

            Value& Value::operator=(const Value& other)
            {
                if (this != &other)
                {
                    if (parent == nullptr)
                    {
                        // We're the root, replace it all.
                        cJSON_Delete(data);
                        data = other.data;
                    }
                    else
                    {
                        cJSON_ReplaceItemInObjectCaseSensitive(parent, data->string, other.data);
                    }
                }

                return *this;
            }

            bool Value::operator==(const std::string& s) const
            {
                return cJSON_IsString(data) && s == data->valuestring;
            }

            bool Value::operator==(int value) const
            {
                return cJSON_IsNumber(data) && value == data->valueint;
            }

            bool Value::operator==(double value) const
            {
                return cJSON_IsNumber(data) && value == data->valuedouble;
            }

            Value::operator const char*() const
            {
                return cJSON_IsString(data) ? data->valuestring : "";
            }

            std::string Value::get_string(const std::string& default_value) const
            {
                auto res = default_value;

                if (cJSON_IsString(data))
                {
                    res = data->valuestring;
                }

                return res;
            }

            int Value::get_int(int default_value) const
            {
                auto res = default_value;

                if (cJSON_IsNumber(data))
                {
                    res = data->valueint;
                }

                return res;
            }

            bool Value::get_bool(bool default_value) const
            {
                auto res = default_value;

                if (cJSON_IsBool(data))
                {
                    res = static_cast<bool>(cJSON_IsTrue(data));
                }

                return res;
            }

            Value::operator int() const
            {
                return cJSON_IsNumber(data) ? data->valueint : 0;
            }

            Value::operator double() const
            {
                return cJSON_IsNumber(data) ? data->valuedouble : 0;
            }

            Value::operator bool() const
            {
                return cJSON_IsBool(data) && cJSON_IsTrue(data);
            }

            int Value::get_array_size() const
            {
                return cJSON_GetArraySize(data);
            }

            std::string Value::get_name() const
            {
                return data->string == nullptr ? "" : data->string;
            }

            void Value::get_member_names(std::vector<std::string>& names) const
            {
                // Get names of this nodes child and its siblings
                cJSON* curr = data;
                if (curr != nullptr && curr->child != nullptr)
                {
                    curr = curr->child;

                    while (curr != nullptr && curr->string != nullptr)
                    {
                        names.emplace_back(curr->string);
                        curr = curr->next;
                    }
                }

            }
        }
    }
}
