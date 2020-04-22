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

        if (is_last())
        {
            std::size_t size = 0;

            try
            {
                size = std::stoul(request_parameters().at("size"));
            }
            catch (...)
            {
            }

            if (size == 0)
            {
                response().reply(std::make_unique<responses::StringResponse>(ResponseCode::
                                                                             Expectation_Failed,
                                                                "Request parameter 'size' must be > 0"),
                                                                false);
            }
            else
            {
                response().reply(std::make_unique<SendBlob>(size), false);
            }
        }
    }
}
