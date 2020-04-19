#include "BlobResponder.h"
#include "SendBlob.h"

namespace http_server_test
{
    using namespace smooth::application::network::http;
    using namespace smooth::application::network::http::regular;

    void BlobResponder::request(IConnectionTimeoutModifier& timeout_modifier,
                                const std::string& url,
                                const std::vector<uint8_t>& content)
    {
        (void)timeout_modifier;
        (void)url;
        (void)content;

        if (request_params.last_part)
        {
            std::size_t size = 0;

            try
            {
                size = std::stoul(request_params.request_parameters->at("size"));
            }
            catch (...)
            {
            }

            if (size == 0)
            {
                request_params.response->reply(std::make_unique<responses::StringResponse>(ResponseCode::
                                                                                           Expectation_Failed,
                                                                "Request parameter 'size' must be > 0"),
                                                                false);
            }
            else
            {
                request_params.response->reply(std::make_unique<SendBlob>(size), false);
            }
        }
    }
}
