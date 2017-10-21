//
// Created by permal on 8/10/17.
//

#pragma once


#include <stdint.h>
#include <mutex>

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
                                std::lock_guard<std::mutex> lock(guard);
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
