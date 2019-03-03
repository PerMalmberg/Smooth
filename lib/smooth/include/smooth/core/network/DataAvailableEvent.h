//
// Created by permal on 7/5/17.
//

#pragma once

#include "PacketReceiveBuffer.h"

namespace smooth
{
    namespace core
    {
        namespace network
        {
            /// Event sent when data is available to the application
            /// \tparam PacketType The type of packet that is available.
            template<typename PacketType>
            class DataAvailableEvent
            {
                public:
                    DataAvailableEvent() = default;

                    explicit DataAvailableEvent(IPacketReceiveBuffer <PacketType>* rx) : rx(rx)
                    {
                    }

                    /// Gets the available data
                    /// \param target The instance that will be assigned the data.
                    /// \return True if the data could be retrieved, otherwise false.
                    bool get(PacketType& target) const
                    {
                        bool res = false;
                        if (rx)
                        {
                            res = rx->get(target);
                        }
                        return res;
                    }

                private:
                    IPacketReceiveBuffer<PacketType>* rx = nullptr;
            };
        }
    }
}