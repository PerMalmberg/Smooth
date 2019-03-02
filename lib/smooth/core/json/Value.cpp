#include <smooth/core/json/Value.h>
#include <smooth/core/logging/log.h>
#include <cstdlib>

using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        namespace json
        {

            /// This constructor takes ownership of the data
            Value::Value()
                    : Value(cJSON_CreateObject(), true)
            {
            }

            /// This constructor conditionally takes ownership of the data
            Value::Value(cJSON* src, bool owning)
                    : data(src), owns_data(owning)
            {
            }

            /// This constructor takes ownership of the data
            Value::Value(const std::string& src)
                    : Value(cJSON_Parse(src.c_str()), true)
            {
            }

            Value::Value(cJSON* parent_node, cJSON* object)
                    : parent(parent_node), data(object)
            {
            }

            Value::Value(const Value& other)
            {
                *this = other;
            }

            Value& Value::operator=(const Value& other)
            {
                data = cJSON_Parse(other.to_string().c_str());
                owns_data = true;
                return *this;
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
                    item = cJSON_CreateObject();
                    cJSON_AddItemToObject(data, key.c_str(), item);
                    return Value(data, item);
                }
            }

            Value Value::operator[](size_t index)
            {
                if (parent == nullptr)
                {
                    throw std::logic_error("Cannot operate on root object");
                }

                // Indexing as an array means we want to access the current object as an array.
                if (!cJSON_IsArray(data))
                {
                    // Remake the item into an array.
                    std::string name = data->string;
                    cJSON_DeleteItemFromObjectCaseSensitive(parent, name.c_str());
                    data = cJSON_CreateArray();
                    cJSON_AddItemToObject(parent, name.c_str(), data);
                }

                // Add any missing items in the array
                auto size = static_cast<size_t>(cJSON_GetArraySize(data));
                if (index >= size)
                {
                    size_t to_add = index - size + 1;
                    for (size_t i = 0; i < to_add; ++i)
                    {
                        cJSON_AddItemToArray(data, cJSON_CreateObject());
                    }
                }

                auto item = cJSON_GetArrayItem(data, static_cast<int>(index));
                return Value(data, item);
            }

            Value& Value::operator=(const char* s)
            {
                if (parent == nullptr)
                {
                    throw std::logic_error("Cannot operate on root object");
                }

                auto new_data = cJSON_CreateString(s);

                if (cJSON_IsArray(parent))
                {
                    cJSON_ReplaceItemViaPointer(parent, data, new_data);
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
                if (parent == nullptr)
                {
                    throw std::logic_error("Cannot operate on root object");
                }

                auto new_data = cJSON_CreateNumber(value);

                if (cJSON_IsArray(parent))
                {
                    cJSON_ReplaceItemViaPointer(parent, data, new_data);
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
                if (parent == nullptr)
                {
                    throw std::logic_error("Cannot operate on root object");
                }

                auto new_data = cJSON_CreateNumber(value);

                if (cJSON_IsArray(parent))
                {
                    cJSON_ReplaceItemViaPointer(parent, data, new_data);
                }
                else
                {
                    cJSON_ReplaceItemInObjectCaseSensitive(parent, data->string, new_data);
                    data = new_data;
                }

                return *this;
            }

            Value& Value::operator=(bool value)
            {
                if (parent == nullptr)
                {
                    throw std::logic_error("Cannot operate on root object");
                }

                auto new_data = cJSON_CreateBool(value);

                if (cJSON_IsArray(parent))
                {
                    cJSON_ReplaceItemViaPointer(parent, data, new_data);
                }
                else
                {
                    cJSON_ReplaceItemInObjectCaseSensitive(parent, data->string, new_data);
                    data = new_data;
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
                    res = cJSON_IsTrue(data) != 0;
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
                std::string res;
                
                if(data)
                {
                    res = data->string == nullptr ? "" : data->string;
                }

                return res;
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

            std::string Value::to_string() const
            {
                auto* p = cJSON_Print(data);
                std::string s{p};
                cJSON_free(p);
                return s;
            }

            void Value::erase(const std::string& name)
            {
                if (data)
                {
                    auto item = cJSON_HasObjectItem(data, name.c_str());
                    if (item)
                    {
                        cJSON_DeleteItemFromObject(data, name.c_str());
                    }
                }
            }

            void Value::erase(int index)
            {
                if (data && cJSON_IsArray(data))
                {
                    auto size = cJSON_GetArraySize(data);
                    if (index >= 0 && index < size)
                    {
                        cJSON_DeleteItemFromArray(data, index);
                    }
                }
            }
        }
    }
}
