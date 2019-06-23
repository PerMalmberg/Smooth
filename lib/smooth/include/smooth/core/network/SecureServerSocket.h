#pragma once

#include "ServerSocket.h"
#include <memory>
#include <vector>
#include <smooth/core/network/SecureSocket.h>
#include <smooth/core/network/MbedTLSContext.h>

namespace smooth::core::network
{
    template<typename Client, typename Protocol>
    class SecureServerSocket
            : public ServerSocket<Client, Protocol>
    {
        public:
            template<typename ...ProtocolArguments>
            static std::shared_ptr<ServerSocket<Client, Protocol>>
            create(smooth::core::Task& task,
                   int max_client_count,
                   int backlog,
                   const std::vector<unsigned char>& ca_chain,
                   const std::vector<unsigned char>& own_cert,
                   const std::vector<unsigned char>& private_key,
                   const std::vector<unsigned char>& password,
                   ProtocolArguments... proto_args);

        protected:
            template<typename ...ProtocolArguments>
            SecureServerSocket(smooth::core::Task& task,
                               int max_client_count,
                               int backlog,
                               const std::vector<unsigned char>& ca_chain,
                               const std::vector<unsigned char>& own_cert,
                               const std::vector<unsigned char>& private_key,
                               const std::vector<unsigned char>& password,
                               ProtocolArguments... proto_args)
                    : ServerSocket<Client, Protocol>(task,
                                                     max_client_count,
                                                     backlog,
                                                     proto_args...)
            {
                server_context.init_server(ca_chain, own_cert, private_key, password);
            }

            void readable(ISocketBackOff& ops) override;

        private:
            MBedTLSContext server_context{};
    };

    template<typename Client, typename Protocol>
    template<typename ...ProtocolArguments>
    std::shared_ptr<ServerSocket<Client, Protocol>>
    SecureServerSocket<Client, Protocol>::create(smooth::core::Task& task,
                                                 int max_client_count,
                                                 int backlog,
                                                 const std::vector<unsigned char>& ca_chain,
                                                 const std::vector<unsigned char>& own_cert,
                                                 const std::vector<unsigned char>& private_key,
                                                 const std::vector<unsigned char>& password,
                                                 ProtocolArguments... proto_args)
    {
        class MakeSharedActivator
                : public SecureServerSocket<Client, Protocol>
        {
            public:
                MakeSharedActivator(smooth::core::Task& task,
                                    int max_client_count,
                                    int backlog,
                                    const std::vector<unsigned char>& ca_chain,
                                    const std::vector<unsigned char>& own_cert,
                                    const std::vector<unsigned char>& private_key,
                                    const std::vector<unsigned char>& password,
                                    ProtocolArguments... proto_args)
                        : SecureServerSocket<Client, Protocol>(task,
                                                               max_client_count,
                                                               backlog,
                                                               ca_chain,
                                                               own_cert,
                                                               private_key,
                                                               password,
                                                               proto_args...)
                {
                }
        };

        return std::make_shared<MakeSharedActivator>(task,
                                                     max_client_count,
                                                     backlog,
                                                     ca_chain,
                                                     own_cert,
                                                     private_key,
                                                     password,
                                                     proto_args...);
    }

    template<typename Client, typename Protocol>
    void SecureServerSocket<Client, Protocol>::readable(ISocketBackOff& ops)
    {
        auto accepted = this->accept_request(ops);
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

            client->set_client_context(this->client_context);
            client->set_socket(socket);
        }
    }
}