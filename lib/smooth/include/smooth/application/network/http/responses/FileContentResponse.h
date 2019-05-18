#pragma once

#include "Response.h"
#include <smooth/core/filesystem/Path.h>
#include <smooth/core/filesystem/Fileinfo.h>

namespace smooth::application::network::http::responses
{
    class FileContentResponse
            : public Response
    {
        public:
            explicit FileContentResponse(smooth::core::filesystem::Path full_path);

            // Called once when beginning to send a response.
            void get_headers(std::unordered_map<std::string, std::string>& headers) override;

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) override;

        private:
            smooth::core::filesystem::Path path;
            smooth::core::filesystem::FileInfo info;
            std::size_t sent{0};
    };
}