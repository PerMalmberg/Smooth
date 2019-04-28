#pragma once


namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                using ResponseSignature = std::function<HTTPPacket(const std::string& url,
                                                                    bool first_part,
                                                                    bool last_part,
                                                                    const std::unordered_map<std::string, std::string>& headers,
                                                                    const std::unordered_map<std::string, std::string>& request_parameters,
                                                                    const std::vector<uint8_t>& content)>;

            }
        }
    }
}