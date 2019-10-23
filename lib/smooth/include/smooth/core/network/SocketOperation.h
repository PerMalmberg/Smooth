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

namespace smooth::core::network
{
    class SocketOperation
    {
        public:
            enum class Op
            {
                Start,
                Stop,
                AddActiveSocket
            };

            SocketOperation() = default;

            SocketOperation(Op op, std::shared_ptr<core::network::ISocket> sock)
                    : op(op), sock(std::move(sock))
            {
            }

            [[nodiscard]] Op get_op() const
            {
                return op;
            }

            [[nodiscard]] std::shared_ptr<core::network::ISocket> get_socket() const
            {
                return sock;
            }

        private:
            Op op = Op::Stop;
            std::shared_ptr<core::network::ISocket> sock{};
    };
}
