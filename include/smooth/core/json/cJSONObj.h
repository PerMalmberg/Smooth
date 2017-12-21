#pragma once

#include <cJSON.h>
#include <memory>

namespace smooth
{
    namespace core
    {
        namespace json
        {
            class cJSONDeleter
            {
                public:
                    cJSONDeleter& operator()(std::unique_ptr<cJSON> item)
                    {
                        if(item)
                        {
                            cJSON_Delete(item.release());
                        }
                    }
            };

            class cJSONObj
            {

                private:
                    std::unique_ptr<cJSON, cJSONDeleter> item{};
            };
        }
    }
}