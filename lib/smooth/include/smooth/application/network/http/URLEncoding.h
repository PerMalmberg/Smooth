#pragma once

#include <string>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                class URLEncoding
                {
                    public:
                        // Decodes a URL, rejecting control characters
                        bool decode(std::string& url);
                        bool decode(std::string::iterator begin, std::string::iterator end);
                    private:
                        inline bool has_two_left(std::size_t total_length, std::size_t current_ix) const;
                };
            }
        }
    }
}

