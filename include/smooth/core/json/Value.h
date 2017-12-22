#pragma once

#include <memory>
#include <string>

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
                    explicit Value(const std::string& src);
                    Value(cJSON* parent, const char* key_name, cJSON* object);

                    Value operator[](const std::string& key);

                    Value& operator=(const std::string& s);
                    Value& operator=(int value);
                    Value& operator=(double value);
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

                    explicit operator std::string() const;
                    explicit operator int() const;
                    explicit operator double() const;

                private:
                    cJSON* parent = nullptr;
                    cJSON* data = nullptr;
                    std::string key{};
            };
        }
    }
}