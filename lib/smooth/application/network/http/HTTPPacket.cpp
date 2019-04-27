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
                HTTPPacket::HTTPPacket(ResponseCode code, const std::string&& version,
                                       const std::unordered_map<std::string, std::string>&& new_headers)
                {
                    append("HTTP/");
                    append(version);
                    append(" ");
                    append(std::to_string(static_cast<int>(code)));
                    append(" ");
                    append("\r\n");

                    for (const auto& header : new_headers)
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