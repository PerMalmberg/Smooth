#pragma once

#include <unordered_map>
#include <memory>
#include "ResponseSignature.h"
#include "HTTPProtocol.h"
#include "HTTPPacket.h"

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                template<int MaxHeaderSize, int ContentChuckSize>
                class IRequestHandler
                {
                    public:
                        virtual ~IRequestHandler() = default;

                        virtual void handle_post(core::network::IPacketSender<HTTPProtocol<MaxHeaderSize, ContentChuckSize>>& sender,
                                                 const std::string& requested_url,
                                                 const std::unordered_map<std::string, std::string>& request_headers,
                                                 const std::unordered_map<std::string, std::string>& request_parameters,
                                                 const std::vector<uint8_t>& data,
                                                 bool fist_part,
                                                 bool last_part) = 0;
                };
            }
        }
    }
}