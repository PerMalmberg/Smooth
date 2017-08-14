//
// Created by permal on 8/10/17.
//

#pragma once


#include <stdint.h>
#include <smooth/core/ipc/Mutex.h>

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
                                smooth::core::ipc::Mutex::Lock lock(guard);
                                return ++id;

                            }
                        private:
                            static smooth::core::ipc::Mutex guard;
                            static uint16_t id;
                    };
                }
            }
        }
    }
}
