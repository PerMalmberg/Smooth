#include <smooth/core/network/CommonSocket.h>
#include <memory>
#include <sys/socket.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/SocketDispatcher.h>

using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        namespace network
        {
            void CommonSocket::clear_socket_id()
            {
                socket_id = INVALID_SOCKET;
            }

            int CommonSocket::get_socket_id() const
            {
                return socket_id;
            }

            bool CommonSocket::is_connected() const
            {
                return connected;
            }

            bool CommonSocket::set_non_blocking()
            {
                bool res = true;

                auto opts = fcntl(socket_id, F_GETFL, 0);
                if (opts < 0)
                {
                    loge("Could not get socket flags");
                    res = false;
                }
                else if (fcntl(socket_id, F_SETFL, opts | O_NONBLOCK) < 0)
                {
                    loge("Could not set non blocking flag");
                    res = false;
                }

                return res;
            }

            void CommonSocket::log(const char* message)
            {
                Log::verbose("Socket",
                             Format("[{1}, {2}, {3}, {4}]: {5}",
                                    Str(ip->get_host()),
                                    Int32(ip->get_port()),
                                    Int32(socket_id),
                                    Pointer(this),
                                    Str(message)));
            }

            void CommonSocket::loge(const char* message)
            {
                Log::error("Socket",
                           Format("[{1}, {2}, {3} {4}]: {5}: {6} ({7})",
                                  Str(ip->get_host()),
                                  Int32(ip->get_port()),
                                  Int32(socket_id),
                                  Pointer(this),
                                  Str(message),
                                  Str(strerror(errno)),
                                  Int32(errno)));
            }

            void CommonSocket::stop()
            {
                stop_internal();
                SocketDispatcher::instance().perform_op(SocketOperation::Op::Stop, shared_from_this());
            }

            bool CommonSocket::is_active() const
            {
                return active;
            }

            bool CommonSocket::restart()
            {
                stop();
                return start(ip);
            }
        }
    }
}