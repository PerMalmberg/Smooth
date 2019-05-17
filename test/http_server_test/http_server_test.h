#pragma once

#include <functional>
#include <memory>
#include <smooth/core/Application.h>
#include <smooth/application/network/http/HTTPServer.h>
#include <smooth/core/network/ServerSocket.h>
#include <smooth/core/network/SecureServerSocket.h>
#include <smooth/application/network/http/HTTPServerClient.h>
#include <smooth/application/network/http/HTTPProtocol.h>

#ifdef ESP_PLATFORM
    #include <smooth/core/filesystem/MMCSDCard.h>
#endif

namespace http_server_test
{
    class App
            : public smooth::core::Application
    {
        public:
            App();

            void init() override;

        private:
#ifdef ESP_PLATFORM
            std::unique_ptr<smooth::core::filesystem::MMCSDCard> sd_card{};
#endif

            static constexpr int MaxHeaderSize = 1024;
            static constexpr int ContentChuckSize = 500;
            using Client = smooth::application::network::http::HTTPServerClient<MaxHeaderSize, ContentChuckSize>;
            using Protocol = smooth::application::network::http::HTTPProtocol<MaxHeaderSize, ContentChuckSize>;

            std::unique_ptr<smooth::application::network::http::HTTPServer<smooth::core::network::ServerSocket<Client, Protocol>, MaxHeaderSize, ContentChuckSize>> insecure_server{};
            std::unique_ptr<smooth::application::network::http::HTTPServer<smooth::core::network::SecureServerSocket<Client, Protocol>, MaxHeaderSize, ContentChuckSize>> secure_server{};
    };
}