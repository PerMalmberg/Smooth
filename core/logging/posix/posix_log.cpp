#include <smooth/core/logging/log.h>
#include <iostream>
#include <mutex>

namespace smooth
{
    namespace core
    {
        namespace logging
        {
            static std::mutex guard;

            void Log::error(const std::string& tag, const Format& fmt)
            {
                std::lock_guard<std::mutex> lock (guard);;
                std::cout << "(E)" << tag << ": " << fmt << std::endl;
            }

            void Log::warning(const std::string& tag, const Format& fmt)
            {
                std::lock_guard<std::mutex> lock(guard);;
                std::cout << "(W)" << tag << ": " << fmt << std::endl;
            }

            void Log::info(const std::string& tag, const Format& fmt)
            {
                std::lock_guard<std::mutex> lock(guard);;
                std::cout << "(I)" << tag << ": " << fmt << std::endl;
            }

            void Log::debug(const std::string& tag, const Format& fmt)
            {
                std::lock_guard<std::mutex> lock(guard);;
                std::cout << "(D)" << tag << ": " << fmt << std::endl;
            }

            void Log::verbose(const std::string& tag, const Format& fmt)
            {
                std::lock_guard<std::mutex> lock(guard);;
                std::cout << "(V)" << tag << ": " << fmt << std::endl;
            }
        };
    }
}
