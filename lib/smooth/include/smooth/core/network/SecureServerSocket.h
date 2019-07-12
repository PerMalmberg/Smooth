// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "ServerSocket.h"
#include <memory>
#include <vector>
#include <smooth/core/network/SecureSocket.h>
#include <smooth/core/network/MbedTLSContext.h>

namespace smooth::core::network
{
    template<typename Client, typename Protocol, typename ClientContext>
    class SecureServerSocket
            : public ServerSocket<Client, Protocol, ClientContext>
    {
        public:
            template<typename ...ProtocolArguments>
            static std::shared_ptr<ServerSocket<Client, Protocol, ClientContext>>
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
                    : ServerSocket<Client, Protocol, ClientContext>(task,
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

    template<typename Client, typename Protocol, typename ClientContext>
    template<typename ...ProtocolArguments>
    std::shared_ptr<ServerSocket<Client, Protocol, ClientContext>>
    SecureServerSocket<Client, Protocol, ClientContext>::create(smooth::core::Task& task,
                                                 int max_client_count,
                                                 int backlog,
                                                 const std::vector<unsigned char>& ca_chain,
                                                 const std::vector<unsigned char>& own_cert,
                                                 const std::vector<unsigned char>& private_key,
                                                 const std::vector<unsigned char>& password,
                                                 ProtocolArguments... proto_args)
    {
        class MakeSharedActivator
                : public SecureServerSocket<Client, Protocol, ClientContext>
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
                        : SecureServerSocket<Client, Protocol, ClientContext>(task,
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

    template<typename Client, typename Protocol, typename ClientContext>
    void SecureServerSocket<Client, Protocol, ClientContext>::readable(ISocketBackOff& ops)
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