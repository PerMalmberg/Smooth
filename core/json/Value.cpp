#include <smooth/core/json/Json.h>
#include <smooth/core/util/make_unique.h>

using namespace smooth::core::util;

namespace smooth
{
    namespace core
    {
        namespace json
        {
            Value::Value(cJSON* src)
            {
                data = src;
            }

            Value::Value(cJSON* parent, const char* key_name, cJSON* object)
            {
                this->parent = parent;
                key = key_name;
                data = object;
            }

            Value Value::operator[](const std::string& key)
            {
                auto item = cJSON_GetObjectItemCaseSensitive(data, key.c_str());

                if(item)
                {
                    return Value(data, key.c_str(), item);
                }
                else
                {
                    auto new_item = cJSON_CreateObject();
                    cJSON_AddItemToObject(data, key.c_str(), new_item);
                    return Value(data, key.c_str(), new_item);
                }
            }

            Value& Value::operator=(const std::string& s)
            {
                if(parent == nullptr)
                {
                    // We're the root, replace it all.
                    cJSON_Delete(data);
                    data = cJSON_CreateString(s.c_str());
                }
                else
                {
                    data = cJSON_CreateString(s.c_str());
                    cJSON_ReplaceItemInObjectCaseSensitive(parent, key.c_str(), data);
                }

                return *this;
            }

            bool Value::operator==(const std::string& s)
            {
                return cJSON_IsString(data) && s == data->valuestring;
            }
        }
    }
}
