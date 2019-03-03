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
                    bool exists() const 
                    {
                         return f.exists();
                    }
                    Value& value() { return v; }

                    void load();
                private:

                    smooth::core::filesystem::File f;
                    Value v{};
            };
        }
    }
}
