#include "UploadResponder.h"
#include "smooth/application/network/http/regular/HTTPHeaderDef.h"
#include "smooth/application/network/http/regular/MIMEParser.h"
#include "smooth/core/filesystem/filesystem.h"

#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>

namespace http_files_upload_test
{
    using namespace smooth::application::network::http;
    using namespace smooth::application::network::http::regular;
    using namespace smooth::core::filesystem;
    using json = nlohmann::json;

    void UploadResponder::start_of_request()
    {
        Log::info("start of request", "alloc");
        pState = static_cast<crypto_generichash_blake2b_state*> (sodium_malloc(crypto_generichash_statebytes()));
        hashes.clear();
    }

    /// This request handler assumed it is being used only to handle either form or URL encoded data in a POST.
    /// As such, it forwards all data to the MIMEParser.
    void UploadResponder::request(IConnectionTimeoutModifier& /*timeout_modifier*/,
                                  const std::string& /*url*/,
                                  const std::vector<uint8_t>& content)
    {
        // Pass content to mime parser with callbacks to handle the data.
        mime.parse(content, *this, *this, static_cast<uint16_t> (4096));
    }

    void UploadResponder::end_of_request()
    {
        std::string hashes_json = hashes.dump();
        response().reply(std::make_unique<responses::StringResponse>(ResponseCode::OK,
                            hashes_json, false), false);

        Log::info("end_of_request", "hashes: {}", hashes_json);
        sodium_free(pState);
    }

    void UploadResponder::form_data(
        const std::string& field_name,
        const std::string& actual_file_name,
        const BoundaryIterator& begin,
        const BoundaryIterator& end,
        const bool file_start,
        const bool file_close)
    {
        if (field_name == "file_to_upload")
        {
            if (file_start)
            {
                crypto_generichash_init(pState, NULL, 0, sizeof hash);

                Log::info("form_data", "File name: {}", actual_file_name);
            }

            auto len = std::distance(begin, end);
            crypto_generichash_update(pState, reinterpret_cast<const unsigned char*>(&*begin),
            static_cast<long long unsigned int>(len));

            if (file_close)
            {
                crypto_generichash_final(pState, hash, sizeof hash);
                std::ostringstream ss;

                for (long unsigned int i = 0; i < sizeof hash; i++)
                {
                    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
                }

                Log::info("form_data", "hash:{}", ss.str());
                hashes[actual_file_name] = ss.str();
            }
        }
    }

    void UploadResponder::url_encoded(std::unordered_map<std::string, std::string>& data)
    {
        if (is_last())
        {
            response().reply(
                std::make_unique<responses::StringResponse>( ResponseCode::OK,
                R"(You entered this text:<br/> <textarea readonly cols="120" rows="20" wrap="soft">)"
                + data["edit_box"] + "</textarea>"),
                false);
        }
    }
}
