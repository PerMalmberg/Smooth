#pragma once

#include <cstdint>
#include <regex>
#include <vector>

namespace smooth::application::network::http
{
    class MIMEParser
    {
        public:
            using ContentCallback = std::function<void(std::string&& name, const std::vector<uint8_t>& content)>;
            using BoundaryIterator = std::vector<uint8_t>::const_iterator;
            using Boundaries = std::vector<BoundaryIterator>;

            bool find_boundary(const std::string&& s);

            void reset() noexcept;

            bool parse(const uint8_t* p, std::size_t length, const ContentCallback& content_callback);

        private:
            auto find_boundaries() const;

            BoundaryIterator get_end_of_boundary(BoundaryIterator start);

            void parse_content(BoundaryIterator begin, BoundaryIterator end, const ContentCallback& content_callback) const;

            const std::regex form_data_pattern{R"!(multipart\/form-data;.*boundary=(.+?)( |$))!"};
            std::vector<uint8_t> boundary{};
            std::vector<uint8_t> end_boundary{};
            std::vector<uint8_t> data{};
    };
}