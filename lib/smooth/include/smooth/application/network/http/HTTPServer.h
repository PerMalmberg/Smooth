#pragma once

#include <memory>
#include "HTTPProtocol.h"
#include <smooth/core/network/ServerSocket.h>
#include <smooth/core/network/SecureServerSocket.h>
#include <smooth/application/network/http/HTTPProtocol.h>
#include <smooth/application/network/http/HTTPServerClient.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace http
            {
                class HTTPServer
                {
                    public:

                    private:
                        std::shared_ptr<smooth::core::network::ServerSocket<HTTPServerClient, HTTPProtocol>> server;
                };
            }
        }
    }
}



