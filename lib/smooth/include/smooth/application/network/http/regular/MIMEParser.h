// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <cstdint>
#include <regex>
#include <vector>
#include <tuple>
#include <unordered_map>

namespace smooth::application::network::http::regular
{
    class MIMEParser
    {
        public:
            using MimeData = std::vector<uint8_t>;
            using BoundaryIterator = MimeData::const_iterator;
            using Boundaries = std::vector<BoundaryIterator>;
            using FormDataCallback = std::function<void(const std::string& field_name, const std::string& actual_file_name, const BoundaryIterator& begin,
                                                       const BoundaryIterator& end)>;
            using URLEncodedDataCallback = std::function<void(std::unordered_map<std::string, std::string>& data)>;

            bool detect_mode(const std::string& content_type, std::size_t content_length);

            void reset() noexcept;

            void parse(const std::vector<uint8_t>& p, const FormDataCallback& content_callback,
                       const URLEncodedDataCallback& url_data)
            {
                parse(p.data(), p.size(), content_callback, url_data);
            }

            void parse(const uint8_t* p, std::size_t length, const FormDataCallback& content_callback,
                       const URLEncodedDataCallback& url_data);

        private:
            enum class Mode
            {
                    None,
                    FormData,
                    FormURLEncoded
            };

            auto find_boundaries() const;

            BoundaryIterator get_end_of_boundary(BoundaryIterator begin);

            void
            parse_content(BoundaryIterator start_of_content, BoundaryIterator end_of_content,
                          const FormDataCallback& content_callback) const;

            void adjust_boundary_beginning_for_crlf(BoundaryIterator start_of_data, Boundaries& found_boundaries) const;

            bool is_crlf(BoundaryIterator start) const;

            std::tuple<BoundaryIterator,
                    std::unordered_map<std::string, std::string>,
                    std::unordered_map<std::string, std::string>>
            consume_headers(BoundaryIterator begin, BoundaryIterator end) const;

            std::vector<uint8_t> boundary{};
            std::vector<uint8_t> end_boundary{};
            std::vector<uint8_t> data{};
            std::unordered_map<std::string, std::string> form_url_encoded_data{};
            const std::regex form_data_pattern{R"!(multipart\/form-data;.*boundary=(.+?)( |$))!"};
            const std::regex url_encoded_pattern{R"!(application\/x-www-form-urlencoded)!"};
            const std::vector<uint8_t> crlf{'\r', '\n'};
            const std::vector<uint8_t> crlf_double{'\r', '\n', '\r', '\n'};
            Mode mode{Mode::None};
            std::size_t expected_content_length{0};

            void parse_content_disposition(const std::unordered_map<std::string, std::string>& headers,
                                           std::unordered_map<std::string, std::string>& content_disposition) const;
    };
}