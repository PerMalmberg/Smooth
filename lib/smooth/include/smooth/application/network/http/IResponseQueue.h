#pragma once

#include <memory>

namespace smooth::application::network::http
{
    class IResponseQueue
    {
        public:
            virtual ~IResponseQueue() = default;

            virtual void reply(std::unique_ptr<responses::IRequestResponseOperation> response) = 0;
    };
}