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
                std::cout << "(E)" << tag << ": " << fmt << "\r\n";
            }

            void Log::warning(const std::string& tag, const Format& fmt)
            {
                std::cout << "(W)" << tag << ": " << fmt << "\r\n";
            }

            void Log::info(const std::string& tag, const Format& fmt)
            {
                std::cout << "(I)" << tag << ": " << fmt << "\r\n";
            }

            void Log::debug(const std::string& tag, const Format& fmt)
            {
                std::cout << "(D)" << tag << ": " << fmt << "\r\n";
            }

            void Log::verbose(const std::string& tag, const Format& fmt)
            {
                std::cout << "(D)" << tag << ": " << fmt << "\r\n";
            }
        };
    }
}
