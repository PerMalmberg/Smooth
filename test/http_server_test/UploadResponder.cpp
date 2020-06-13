#include "UploadResponder.h"
#include "smooth/application/network/http/regular/HTTPHeaderDef.h"
#include "smooth/application/network/http/regular/MIMEParser.h"
#include "smooth/core/filesystem/filesystem.h"

namespace http_server_test
{
    using namespace smooth::application::network::http;
    using namespace smooth::application::network::http::regular;
    using namespace smooth::core::filesystem;

    /// This request handler assumed it is being used only to handle either form or URL encoded data in a POST.
    /// As such, it forwards all data to the MIMEParser.
    void UploadResponder::request(IConnectionTimeoutModifier& timeout_modifier,
                                  const std::string& url,
                                  const std::vector<uint8_t>& content)
    {
        (void)timeout_modifier;
        (void)url;

        // Pass content to mime parser with callbacks to handle the data.
        mime.parse(content, *this, *this, static_cast<uint16_t> (4096));
    }

    void UploadResponder::form_data(
        const std::string& field_name,
        const std::string& actual_file_name,
        const smooth::application::network::http::regular::BoundaryIterator& begin,
        const smooth::application::network::http::regular::BoundaryIterator& end,
        const bool file_start,
        const bool file_close)
    {
        if (field_name == "file_to_upload" || field_name == "second_file_to_upload")
        {
            if (file_start)
            {
                // Store the file in web_root/uploads
                path = uploads / actual_file_name;

                // Log::info("form_data", "File name: {}", path.str());
                create_directory(path.parent());

                if (FileInfo{ path }.exists())
                {
                    // Log::info("form_data", "File exists");
                    remove(path);
                }

                // Log::info("form_data", "before open");
                to_save.open(path.str(), std::ios::out | std::ios::app | std::ios::binary);
                Log::info("form_data", "File opened");
            }

            auto len = std::distance(begin, end);

            // Log::info("form_data", "chunk to write: {},", static_cast<int>(len));
            to_save.write(reinterpret_cast<const char*>(&*begin), static_cast<int>(len));

            // Log::info("form_data", "data written");

            if (file_close)
            {
                to_save.close();
                Log::info("form_data", "file close: {}", path.str());
            }

            if (is_last())
            {
                response().reply(std::make_unique<responses::StringResponse>(
                    ResponseCode::OK, "File(s) have been stored to "
                                                                                           + uploads.str()), false);
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
