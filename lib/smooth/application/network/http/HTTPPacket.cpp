#include <smooth/application/network/http/HTTPPacket.h>

#include <string>
#include <unordered_map>
#include <algorithm>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                HTTPPacket::HTTPPacket(Method method, const std::string& path,
                                       const std::unordered_map<std::string, std::string>& headers)
                {
                    if (method == Method::GET)
                    {
                        append(http_get);
                    }
                    else
                    {
                        append(http_post);
                    }

                    append(" ");
                    append(path);
                    append(" ");
                    append(http_1_0);
                    append("\r\n");

                    for (const auto& header : headers)
                    {
                        const auto& key = header.first;
                        const auto& value = header.second;

                        append(key);
                        append(": ");
                        append(value);
                        append("\r\n");
                    }

                    // Add required ending CRLF
                    append("\r\n\r\n");
                }

                void HTTPPacket::append(const std::string& s)
                {
                    std::copy(s.begin(), s.end(), std::back_inserter(content));
                }
            }
        }
    }
}