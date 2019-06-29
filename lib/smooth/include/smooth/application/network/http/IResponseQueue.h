#pragma once

#include <memory>
#include "responses/IRequestResponeOperation.h"

namespace smooth::application::network::http
{
    class IResponseQueue
    {
        public:
            virtual ~IResponseQueue() = default;

            virtual void reply(std::unique_ptr<responses::IRequestResponseOperation> response) = 0;
            virtual void reply_error(std::unique_ptr<responses::IRequestResponseOperation> response) = 0;
    };
}