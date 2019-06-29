#pragma once

#include "StringResponse.h"

namespace smooth::application::network::http::responses
{
    class ErrorResponse
            : public StringResponse
    {
        public:
            explicit ErrorResponse(ResponseCode code);
    };
}