#pragma once

namespace smooth
{
    namespace core
    {
        namespace network
        {
            enum class NetworkEvent
            {
                    GOT_IP,
                    DISCONNECTED
            };

            class NetworkStatus
            {
                public:
                    NetworkStatus() = default;

                    NetworkStatus(NetworkEvent event, bool ip_changed)
                            : event(event), ip_changed(ip_changed)
                    {
                    }

                    NetworkStatus(const NetworkStatus&) = default;
                    NetworkStatus& operator=(const NetworkStatus&) = default;

                    NetworkEvent event;
                    bool ip_changed;
            };
        }
    }
}