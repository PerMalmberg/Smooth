//
// Created by permal on 7/22/17.
//

#pragma once

#include <chrono>
#include <smooth/application/network/mqtt/packet/MQTTProtocol.h>

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
                    class Connect
                            : public MQTTProtocol
                    {
                        public:
                            Connect() = default;
                            Connect(const std::string& client_id, std::chrono::seconds keep_alive, bool clean_session = true);

                            bool get_clean_session();

                        protected:
                            bool has_payload() const override
                            {
                                return true;
                            }

                        private:
                            bool clean_session = true;
                    };
                }
            }
        }
    }
}
