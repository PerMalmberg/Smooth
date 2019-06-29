
#include <smooth/application/network/http/responses/ErrorResponse.h>
#include <smooth/application/network/http/HTTPHeaderDef.h>

namespace smooth::application::network::http::responses
{
    ErrorResponse::ErrorResponse(ResponseCode code)
            : smooth::application::network::http::responses::StringResponse(
            code, std::string{std::to_string(static_cast<int>(code))} + " " + response_code_to_text.at(code))
    {
    }
}