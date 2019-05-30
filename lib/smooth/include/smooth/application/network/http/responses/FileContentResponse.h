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

            // Called at least once when sending a response and until ResponseStatus::AllSent is returned
            ResponseStatus get_data(std::size_t max_amount, std::vector<uint8_t>& target) override;

            void dump() const override;

        private:
            smooth::core::filesystem::Path path;
            smooth::core::filesystem::FileInfo info;
            std::size_t sent{0};
    };
}