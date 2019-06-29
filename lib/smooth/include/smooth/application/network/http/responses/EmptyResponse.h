#pragma once

#include "StringResponse.h"

namespace smooth::application::network::http::responses
{
    class EmptyResponse
            : public StringResponse
    {
        public:
            explicit EmptyResponse(ResponseCode code);
    };
}