//
// Created by permal on 8/10/17.
//

#pragma once


#include <stdint.h>

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
                                return ++id;
                            }
                        private:
                            static uint16_t id;
                    };
                }
            }
        }
    }
}
