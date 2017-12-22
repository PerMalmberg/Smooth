#pragma once

#include <memory>

namespace smooth
{
    namespace core
    {
        namespace json
        {
            class Value
            {
                public:
                    explicit Value(cJSON* src);
                    Value(cJSON* parent, const char* key_name, cJSON* object);

                    Value operator[](const std::string& key);
                    Value& operator=(const std::string& s);
                    bool operator==(const std::string& s);

                private:
                    cJSON* parent = nullptr;
                    cJSON* data = nullptr;
                    std::string key{};
            };
        }
    }
}