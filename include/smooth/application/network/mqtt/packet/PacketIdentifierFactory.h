//
// Created by permal on 8/10/17.
//

#pragma once


#include <stdint.h>
#include <smooth/core/ipc/Lock.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                namespace packet
                {
                    class PacketIdentifierFactory
                    {
                        public:
                            static uint16_t get_id()
                            {
                                smooth::core::ipc::Lock lock(guard);
                                return ++id;

                            }
                        private:
                            static std::mutex guard;
                            static uint16_t id;
                    };
                }
            }
        }
    }
}
