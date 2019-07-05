#pragma once

#include <string>

namespace smooth::application::network::http
{
    class URLEncoding
    {
        public:
            // Decodes a URL, rejecting control characters
            bool decode(std::string& url);

            bool decode(std::string& url, std::string::iterator begin, std::string::iterator end);
    };
}

