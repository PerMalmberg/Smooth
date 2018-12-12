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
                    NetworkStatus(const NetworkEvent event, const bool ip_changed)
                            : event(event), ip_changed(ip_changed)
                    {
                    }

                    NetworkStatus() = default;
                    NetworkStatus(const NetworkStatus&) = default;
                    NetworkStatus& operator=(const NetworkStatus&) = default;

                    NetworkEvent get_event() const { return event; }
                    bool get_ip_changed() const { return ip_changed; }

                private:
                    NetworkEvent event{NetworkEvent::DISCONNECTED};
                    bool ip_changed{false};
            };
        }
    }
}