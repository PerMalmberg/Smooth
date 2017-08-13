//
// Created by permal on 8/10/17.
//

#pragma once


#include <stdint.h>
#include <smooth/core/ipc/Mutex.h>
#include "esp_log.h"

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
                                ESP_LOGV("PacketIdentifierFactory", "%d", id+1);
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
