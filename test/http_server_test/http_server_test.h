#pragma once

#include <functional>
#include <smooth/core/Application.h>
#include <smooth/application/network/http/HTTPServer.h>
#include <smooth/core/network/ServerSocket.h>
#include <smooth/core/network/SecureServerSocket.h>
#include <smooth/application/network/http/HTTPServerClient.h>
#include <smooth/application/network/http/HTTPProtocol.h>


namespace http_server_test
{
    class App
            : public smooth::core::Application
    {
        public:
            App();

            void init() override;

        private:
            static constexpr int MaxPacketSize = 500;
            static constexpr int ContentChuckSize = 500;
            using Client = smooth::application::network::http::HTTPServerClient<MaxPacketSize, ContentChuckSize>;
            using Protocol = smooth::application::network::http::HTTPProtocol<MaxPacketSize, ContentChuckSize>;

            std::unique_ptr<smooth::application::network::http::HTTPServer<smooth::core::network::ServerSocket<Client, Protocol>, MaxPacketSize, ContentChuckSize>> insecure_server{};
            std::unique_ptr<smooth::application::network::http::HTTPServer<smooth::core::network::SecureServerSocket<Client, Protocol>, MaxPacketSize, ContentChuckSize>> secure_server{};
    };
}