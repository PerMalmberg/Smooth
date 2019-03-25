#include <utility>

//
// Created by permal on 10/29/17.
//

#pragma once

namespace smooth
{
    namespace core
    {
        namespace network
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

                    Op get_op() const
                    {
                        return op;
                    }

                    std::shared_ptr<core::network::ISocket> get_socket() const
                    {
                        return sock;
                    }

                private:
                    Op op = Op::Stop;
                    std::shared_ptr<core::network::ISocket> sock{};

            };

        }
    }
}
