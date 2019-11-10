/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include "ServerSocket.h"
#include <memory>
#include <vector>
#include "smooth/core/network/SecureSocket.h"
#include "smooth/core/network/MbedTLSContext.h"
#include "smooth/core/util/create_protected.h"

namespace smooth::core::network
{
    template<typename Client, typename Protocol, typename ClientContext>
    class SecureServerSocket
        : public ServerSocket<Client, Protocol, ClientContext>
    {
        public:
            template<typename... ProtocolArguments>
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
            template<typename... ProtocolArguments>
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
    template<typename... ProtocolArguments>
    std::shared_ptr<ServerSocket<Client, Protocol, ClientContext>> SecureServerSocket<Client, Protocol,
                                                                                      ClientContext>::create(
        smooth::core::Task& task,
        int max_client_count,
        int backlog,
        const std::vector<unsigned char>& ca_chain,
        const std::vector<unsigned char>& own_cert,
        const std::vector<unsigned char>& private_key,
        const std::vector<unsigned char>& password,
        ProtocolArguments... proto_args)
    {
        return smooth::core::util::create_protected_shared<SecureServerSocket<Client, Protocol, ClientContext>>(
                task,
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
        const auto& [ip, accepted_socket_id] = this->accept_request(ops);

        if (ip)
        {
            auto client = this->pool.get();
            auto socket = SecureSocket<Protocol>::create(ip,
                                                         accepted_socket_id,
                                                         client->get_buffers(),
                                                         server_context.create_context(),
                                                         client->get_send_timeout());

            client->set_client_context(this->client_context);
            client->set_socket(socket);
        }
    }
}
