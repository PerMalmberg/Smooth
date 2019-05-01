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
                HTTPPacket::HTTPPacket(ResponseCode code, const std::string& version,
                                       const std::unordered_map<std::string, std::string>& new_headers,
                                       const std::vector<uint8_t>& response_content)
                {
                    append("HTTP/");
                    append(version);
                    append(" ");
                    append(std::to_string(static_cast<int>(code)));
                    append(" ");
                    append(response_code_to_text.at(code));
                    append("\r\n");

                    for (const auto& header : new_headers)
                    {
                        const auto& key = header.first;
                        const auto& value = header.second;

                        add_header(key, value);
                    }

                    add_header("Content-Length", std::to_string(response_content.size()));

                    // Add required ending CRLF
                    append("\r\n");

                    if(!response_content.empty())
                    {
                        std::copy(response_content.begin(), response_content.end(), std::back_inserter(content));
                    }
                }

                void HTTPPacket::append(const std::string& s)
                {
                    std::copy(s.begin(), s.end(), std::back_inserter(content));
                }

                void HTTPPacket::add_header(const std::string& key, const std::string& value)
                {
                    append(key);
                    append(": ");
                    append(value);
                    append("\r\n");
                }

                HTTPPacket::HTTPPacket(ResponseCode code, const std::string& version,
                                       const std::unordered_map<std::string, std::string>& new_headers,
                                       const std::string& response_content) : HTTPPacket(code, version, new_headers, std::vector<uint8_t>())
                {
                    std::copy(response_content.begin(), response_content.end(), std::back_inserter(content));
                }
            }
        }
    }
}