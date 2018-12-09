#pragma once

#include <string>
#include <smooth/core/filesystem/File.h>
#include "Value.h"

namespace smooth
{
    namespace core
    {
        namespace json
        {
            class JsonFile
            {
                public:
                    JsonFile(std::string full_path);
                    
                    bool save() const;
                    Value& value() { return v; }

                private:
                    void load();

                    smooth::core::filesystem::File f;
                    Value v{R"({"a":"b"})"};

            };
        }
    }
}
