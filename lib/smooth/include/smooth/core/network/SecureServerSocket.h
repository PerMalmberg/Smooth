#pragma once

#include "ServerSocket.h"
#include <memory>
#include <vector>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            template<typename Client, typename Protocol>
            class SecureServerSocket
                    : public ServerSocket<Client, Protocol>
            {
                public:
                    static std::shared_ptr<ServerSocket < Client, Protocol>>
                    create(
                    smooth::core::Task& task,
                    int max_client_count,
                    const std::vector<unsigned char>& ca_chain,
                    const std::vector<unsigned char>& own_cert,
                    const std::vector<unsigned char>& private_key,
                    const std::vector<unsigned char>& password
                    );

                protected:
                    SecureServerSocket(smooth::core::Task& task,
                                       int max_client_count,
                                       const std::vector<unsigned char>& ca_chain,
                                       const std::vector<unsigned char>& own_cert,
                                       const std::vector<unsigned char>& private_key,
                                       const std::vector<unsigned char>& password)
                            : ServerSocket<Client, Protocol>(task, max_client_count)
                    {
                        server_context.init_server(ca_chain, own_cert, private_key, password);
                    }

                    void readable() override;

                private:
                    MBedTLSContext server_context{};
            };

            template<typename Client, typename Protocol>
            std::shared_ptr<ServerSocket < Client, Protocol>>
            SecureServerSocket<Client, Protocol>::create(smooth::core::Task& task,
                                                        int max_client_count,
                                                        const std::vector<unsigned char>& ca_chain,
                                                        const std::vector<unsigned char>& own_cert,
                                                        const std::vector<unsigned char>& private_key,
                                                        const std::vector<unsigned char>& password)
        {
            class MakeSharedActivator
                    : public SecureServerSocket<Client, Protocol>
            {
                public:
                    MakeSharedActivator(smooth::core::Task& task,
                                        int max_client_count,
                                        const std::vector<unsigned char>& ca_chain,
                                        const std::vector<unsigned char>& own_cert,
                                        const std::vector<unsigned char>& private_key,
                                        const std::vector<unsigned char>& password)
                            : SecureServerSocket<Client, Protocol>(task,
                                                                   max_client_count,
                                                                   ca_chain,
                                                                   own_cert,
                                                                   private_key,
                                                                   password)
                    {
                    }
            };

            return std::make_shared<MakeSharedActivator>(task,
                                                         max_client_count,
                                                         ca_chain,
                                                         own_cert,
                                                         private_key,
                                                         password);
        }

        template<typename Client, typename Protocol>
        void SecureServerSocket<Client, Protocol>::readable()
        {
            auto accepted = this->accept_request();
            auto ip = std::get<0>(accepted);
            auto socket_id = std::get<1>(accepted);

            if (ip)
            {
                auto client = this->pool.get();
                auto socket = SecureSocket<Protocol>::create(ip,
                                                             socket_id,
                                                             client->get_buffers(),
                                                             server_context.create_context(),
                                                             client->get_send_timeout());

                client->set_socket(socket);
            }
        }
    }
}
}