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
                    private:
                        inline bool has_two_left(std::size_t total_length, std::size_t current_ix) const;
                };
            }
        }
    }
}

