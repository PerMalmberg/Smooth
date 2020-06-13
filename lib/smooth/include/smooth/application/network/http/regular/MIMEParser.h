/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <cstdint>
#include <regex>
#include <vector>
#include <tuple>
#include <unordered_map>

namespace smooth::application::network::http::regular
{
    using MimeData = std::vector<uint8_t>;
    using BoundaryIterator = MimeData::const_iterator;
    using Boundaries = std::vector<BoundaryIterator>;

    class IFormData
    {
        public:
            virtual ~IFormData() = default;

            virtual void form_data(const std::string& field_name,
                                   const std::string& actual_file_name,
                                   const BoundaryIterator& begin,
                                   const BoundaryIterator& end,
                                   const bool file_start,
                                   const bool file_close) = 0;
    };

    class IURLEncodedData
    {
        public:
            virtual ~IURLEncodedData() = default;

            virtual void url_encoded(std::unordered_map<std::string, std::string>& data) = 0;
    };

    static const uint8_t LEN_OF_CRLF = 2;
    const std::vector<uint8_t> equal{ '=' };

    class MIMEParser
    {
        public:
            using MimeData = std::vector<uint8_t>;
            using BoundaryIterator = MimeData::const_iterator;
            using Boundaries = std::vector<BoundaryIterator>;
            using FormDataCallback = std::function<void (const std::string& field_name,
                                                         const std::string& actual_file_name,
                                                         const BoundaryIterator& begin,
                                                         const BoundaryIterator& end,
                                                         const bool first_part,
                                                         const bool last_part)>;
            using URLEncodedDataCallback = std::function<void (std::unordered_map<std::string, std::string>& data)>;

            bool detect_mode(const std::string& content_type, std::size_t content_length);

            void reset() noexcept;

            void parse(const std::vector<uint8_t>& p, IFormData& form_data,
                       IURLEncodedData& url_data, const uint16_t chunksize)
            {
                parse(p.data(), p.size(), form_data, url_data, chunksize);
            }

            void parse(const uint8_t* p, std::size_t length,
                       IFormData& form_data,
                       IURLEncodedData& url_data,
                       const uint16_t chunksize);

        private:
            enum class Mode
            {
                None,
                FormData,
                FormURLEncoded
            };

            enum class ParseStatus
            {
                Begin,
                Headers,
                Data
            };

            auto find_boundary() const;

            auto find_end_boundary() const;

            BoundaryIterator get_end_of_boundary(BoundaryIterator begin);

            bool is_crlf(BoundaryIterator start) const;

            std::tuple<BoundaryIterator,
                       std::unordered_map<std::string, std::string>,
                       std::unordered_map<std::string, std::string>>
            consume_headers(BoundaryIterator begin, BoundaryIterator end) const;

            std::string id{};
            std::string filename{};
            bool first_part{ false };
            bool end_of_transmission{ false };
            std::vector<uint8_t> boundary{};
            std::vector<uint8_t> end_boundary{};
            std::vector<uint8_t> data{};
            std::unordered_map<std::string, std::string> form_url_encoded_data{};
            const std::regex form_data_pattern{ R"!(multipart\/form-data;.*boundary=(.+?)( |$))!" };
            const std::regex url_encoded_pattern{ R"!(application\/x-www-form-urlencoded)!" };
            const std::vector<uint8_t> crlf{ '\r', '\n' };
            const std::vector<uint8_t> crlf_double{ '\r', '\n', '\r', '\n' };
            Mode mode{ Mode::None };
            std::size_t expected_content_length{ 0 };
            ParseStatus parse_status{ ParseStatus::Begin };

            void parse_content_disposition(const std::unordered_map<std::string, std::string>& headers,
                                           std::unordered_map<std::string, std::string>& content_disposition) const;
    };
}
