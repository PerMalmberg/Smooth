#include <smooth/core/logging/log.h>
#include "esp_log.h"

namespace smooth
{
    namespace core
    {
        namespace logging
        {

            void Log::error(const std::string& tag, const Format& fmt)
            {
                ESP_LOGE(tag.c_str(), "%s", fmt.get());
            }

            void Log::warning(const std::string& tag, const Format& fmt)
            {
                ESP_LOGW(tag.c_str(), "%s", fmt.get());
            }

            void Log::info(const std::string& tag, const Format& fmt)
            {
                ESP_LOGI(tag.c_str(), "%s", fmt.get());
            }

            void Log::debug(const std::string& tag, const Format& fmt)
            {
                ESP_LOGD(tag.c_str(), "%s", fmt.get());
            }

            void Log::verbose(const std::string& tag, const Format& fmt)
            {
                ESP_LOGV(tag.c_str(), "%s", fmt.get());
            }
        };
    }
}
