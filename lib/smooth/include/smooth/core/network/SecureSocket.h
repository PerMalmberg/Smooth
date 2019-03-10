#pragma once

#include "Socket.h"

namespace smooth
{
    namespace core
    {
        namespace network
        {
            template<typename Packet>
            class SecureSocket
                    : public Socket<Packet>
            {
                public:
                    friend class smooth::core::network::SocketDispatcher;

                    /// Creates a socket for network communication, with the specified packet type.
                    /// \param tx_buffer The transmit buffer where outgoing packets are put by the application.
                    /// \param rx_buffer The receive buffer used when receiving data
                    /// \param tx_empty The response queue onto which events are put when all outgoing packets are sent.
                    /// These events are forwarded to the application via the response method.
                    /// \param data_available The response queue onto which events are put when data is available. These
                    /// These events are forwarded to the application via the response method.
                    /// \param connection_status The response queue into which events are put when a change in the connection
                    /// state is detected. These events are forwarded to the application via the response method.
                    /// \param send_timeout The amount of time to wait for outgoing data to actually be sent to remote
                    /// endpoint (i.e. the maximum time between send() being called and the socket being writable again).
                    /// If this time is exceeded, the socket will be closed.
                    /// \return a std::shared_ptr pointing to an instance of a ISocket object, or nullptr if no socket could be
                    /// created.
                    static std::shared_ptr<ISocket>
                    create(IPacketSendBuffer<Packet>& tx_buffer, IPacketReceiveBuffer<Packet>& rx_buffer,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status,
                           std::chrono::milliseconds send_timeout = std::chrono::milliseconds(1500));

                protected:
                    SecureSocket(IPacketSendBuffer<Packet>& tx_buffer, IPacketReceiveBuffer<Packet>& rx_buffer,
                                 smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                 smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                 smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status,
                                 std::chrono::milliseconds send_timeout)
                            : Socket<Packet>(tx_buffer, rx_buffer, tx_empty, data_available, connection_status,
                                             send_timeout)
                    {

                    }

                    bool create_socket() override;

                    bool internal_start() override;

                    void read_data() override;

                    void write_data() override;

                private:



            };

            template<typename Packet>
            std::shared_ptr<ISocket> SecureSocket<Packet>::create(IPacketSendBuffer<Packet>& tx_buffer,
                                                                  IPacketReceiveBuffer<Packet>& rx_buffer,
                                                                  smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                                                  smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                                                  smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status,
                                                                  std::chrono::milliseconds send_timeout)
            {

                // This class is solely used to enabled access to the protected SecureSocket<Packet> constructor from std::make_shared<>
                class MakeSharedActivator
                        : public SecureSocket<Packet>
                {
                    public:
                        MakeSharedActivator(IPacketSendBuffer<Packet>& tx_buffer,
                                            IPacketReceiveBuffer<Packet>& rx_buffer,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status,
                                            std::chrono::milliseconds send_timeout)
                                : SecureSocket<Packet>(tx_buffer, rx_buffer, tx_empty, data_available,
                                                       connection_status,
                                                       send_timeout)
                        {
                        }

                };

                std::shared_ptr<ISocket> s = std::make_shared<MakeSharedActivator>(tx_buffer,
                                                                                   rx_buffer,
                                                                                   tx_empty,
                                                                                   data_available,
                                                                                   connection_status,
                                                                                   send_timeout);
                return s;
            }

            template<typename Packet>
            bool SecureSocket<Packet>::create_socket()
            {
                auto res = Socket<Packet>::create_socket();


                return res;
            }

            template<typename Packet>
            bool SecureSocket<Packet>::internal_start()
            {
                auto res = Socket<Packet>::internal_start();

                if (res)
                {

                }

                return res;
            }

            template<typename Packet>
            void SecureSocket<Packet>::read_data()
            {
                Socket<Packet>::read_data();
                // Read decrypted bytes from from 'incoming' and write then to the target
            }

            template<typename Packet>
            void SecureSocket<Packet>::write_data()
            {
                Socket<Packet>::write_data();
            }

        }
    }
}