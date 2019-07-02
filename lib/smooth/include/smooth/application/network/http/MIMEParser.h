#pragma once

#include <cstdint>
#include <regex>
#include <vector>
#include <tuple>
#include <unordered_map>

namespace smooth::application::network::http
{
    class MIMEParser
    {
        public:
            using MimeData = std::vector<uint8_t>;
            using BoundaryIterator = MimeData::const_iterator;
            using Boundaries = std::vector<BoundaryIterator>;
            using ContentCallback = std::function<void(std::string&& name, const BoundaryIterator& start,
                                                       const BoundaryIterator& end)>;

            bool find_boundary(const std::string&& s);

            void reset() noexcept;

            bool parse(const uint8_t* p, std::size_t length, const ContentCallback& content_callback);

        private:
            auto find_boundaries() const;

            BoundaryIterator get_end_of_boundary(BoundaryIterator begin);

            void
            parse_content(BoundaryIterator start_of_content, BoundaryIterator end_of_content,
                          const ContentCallback& content_callback) const;

            void adjust_boundary_begining_for_crlf(BoundaryIterator start_of_data, Boundaries& found_boundaries) const;

            bool is_crlf(BoundaryIterator start) const;

            std::tuple<BoundaryIterator, std::unordered_map<std::string, std::string>>
            consume_headers(BoundaryIterator begin, BoundaryIterator end) const;

            std::vector<uint8_t> boundary{};
            std::vector<uint8_t> end_boundary{};
            std::vector<uint8_t> data{};
            const std::regex form_data_pattern{R"!(multipart\/form-data;.*boundary=(.+?)( |$))!"};
            const std::vector<uint8_t> crlf{'\r', '\n'};
            const std::vector<uint8_t> crlf_double{'\r', '\n', '\r', '\n'};
    };
}