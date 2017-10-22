#include <smooth/core/logging/log.h>
#include <iostream>

namespace smooth
{
    namespace core
    {
        namespace logging
        {
            void Log::error(const std::string& tag, const Format& fmt)
            {
                std::cout << "(E)" << tag << ": " << fmt << std::endl;
            }

            void Log::warning(const std::string& tag, const Format& fmt)
            {
                std::cout << "(W)" << tag << ": " << fmt << std::endl;
            }

            void Log::info(const std::string& tag, const Format& fmt)
            {
                std::cout << "(I)" << tag << ": " << fmt << std::endl;
            }

            void Log::debug(const std::string& tag, const Format& fmt)
            {
                std::cout << "(D)" << tag << ": " << fmt << std::endl;
            }

            void Log::verbose(const std::string& tag, const Format& fmt)
            {
                std::cout << "(D)" << tag << ": " << fmt << std::endl;
            }
        };
    }
}
