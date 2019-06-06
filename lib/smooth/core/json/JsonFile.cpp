#include <smooth/core/json/JsonFile.h>
#include <cJSON.h>

#include <memory>
#include <vector>
#include <iostream>

namespace smooth
{
    namespace core
    {
        namespace json
        {
            JsonFile::JsonFile(std::string full_path)
                    : f(std::move(full_path))
            {
                load();
            }

            void JsonFile::load()
            {
                if(exists())
                {
                    std::vector<uint8_t> data;
                    f.read(data);

                    if(!data.empty())
                    {
                        // Append terminating zero.
                        data.push_back(0);
                        v = Value{cJSON_Parse(reinterpret_cast<const char*>(data.data())), true};
                    }
                }
            }

            bool JsonFile::save() const
            {
                return f.write(v.to_string());
            }
        }
    }
}

