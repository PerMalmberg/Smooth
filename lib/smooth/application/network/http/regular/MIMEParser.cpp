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

#include "smooth/application/network/http/regular/MIMEParser.h"
#include <vector>
#include "smooth/core/util/split.h"
#include "smooth/core/util/string_util.h"
#include "smooth/application/network/http/URLEncoding.h"
#include "smooth/application/network/http/regular/HTTPMethod.h"
#include "smooth/application/network/http/regular/HTTPHeaderDef.h"

#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;
using namespace smooth::core::util;
using namespace smooth::core;

namespace smooth::application::network::http::regular
{
    void MIMEParser::reset() noexcept
    {
        boundary.clear();
        end_boundary.clear();
        form_url_encoded_data.clear();
        data.clear();
        expected_content_length = 0;
        mode = Mode::None;
        end_of_transmission = false;
        parse_status = ParseStatus::Begin;
    }

    bool MIMEParser::detect_mode(const std::string& content_type, std::size_t content_length)
    {
        std::smatch match;

        if (std::regex_match(content_type.begin(), content_type.end(), match, form_data_pattern))
        {
            auto b = match[1].str();

            // https://www.w3.org/Protocols/rfc1341/7_2_Multipart.html
            // Note that the encapsulation boundary must occur at the beginning of a line, i.e., following a CRLF,
            // and that that initial CRLF is considered to be part of the encapsulation boundary rather than part
            // of the preceding part. The boundary must be followed immediately either by another CRLF and the header
            // fields for the next part, or by two CRLFs, in which case there are no header fields for the next part
            // (and it is therefore assumed to be of Content-Type text/plain).

            // Note that we don't include the leading CRLF in the boundary pattern - that is handled separately
            // so that we also match the very first boundary after the HTTP-headers.
            boundary = { b.begin(), b.end() };
            boundary.insert(boundary.begin(), '-');
            boundary.insert(boundary.begin(), '-');
            end_boundary = boundary;

            // The ending boundary is the same as the normal one, but suffixed by "--\r\n" instead of just \r\n
            end_boundary.emplace_back('-');
            end_boundary.emplace_back('-');
            end_boundary.emplace_back('\r');
            end_boundary.emplace_back('\n');

            mode = Mode::FormData;
        }
        else if (std::regex_match(content_type.begin(), content_type.end(), match, url_encoded_pattern))
        {
            mode = Mode::FormURLEncoded;
        }

        expected_content_length = content_length;

        return mode != Mode::None;
    }

    auto MIMEParser::find_boundary() const
    {
        BoundaryIterator p = data.cend();

        if (data.size() > boundary.size() + LEN_OF_CRLF)
        {
            p = std::search(data.cbegin(), data.cend(), boundary.cbegin(), boundary.cend());
        }

        return p;
    }

    auto MIMEParser::find_end_boundary() const
    {
        BoundaryIterator p = data.cend();

        if (data.size() >= end_boundary.size())
        {
            p = std::search(data.cbegin(), data.cend(), end_boundary.cbegin(), end_boundary.cend());
        }

        return p;
    }

    void MIMEParser::parse(const uint8_t* p, std::size_t length, IFormData& form_data, IURLEncodedData& url_data,
                           const uint16_t chunksize)
    {
        BoundaryIterator begin{};
        BoundaryIterator end{};
        bool get_more_data = false;

        for (std::size_t i = 0; i < length; ++i)
        {
            data.emplace_back(p[i]);
        }

        // data.reserve(data.size()+length+1);
        // std::copy(&p[0], &p[length], data.end());
        // Log::info("MIMEParser::myparse", "length={}; data.size()={}", length, data.size());

        if (mode == Mode::FormData)
        {
            while (!end_of_transmission && !get_more_data)
            {
                if (parse_status == ParseStatus::Begin)
                {
                    begin = find_boundary();

                    if (begin != data.cend())  // begin boundary found
                    {
                        parse_status = ParseStatus::Headers;

                        // "+LEN_OF_CRLF": also delete trailing crlf
                        data.erase(data.begin(), get_end_of_boundary(begin) + LEN_OF_CRLF);
                    }
                    else
                    {
                        get_more_data = true;
                    }
                }
                else if (parse_status == ParseStatus::Headers)
                {
                    auto p = std::search(data.cbegin(), data.cend(), crlf_double.cbegin(), crlf_double.cend());

                    if (p != data.cend())  // end of headers found
                    {
                        auto [new_start_of_content, headers,
                              content_disposition] = consume_headers(data.cbegin(), p + 2 * LEN_OF_CRLF);
                        id = content_disposition["name"];
                        filename = content_disposition["filename"];

                        // Delete two CRLF
                        data.erase(data.begin(), p + 2 * LEN_OF_CRLF);
                        parse_status = ParseStatus::Data;
                        first_part = true;
                    }
                    else
                    {
                        get_more_data = true;
                    }
                }
                else if (parse_status == ParseStatus::Data)
                {
                    auto b = std::search(data.cbegin(), data.cend(), boundary.cbegin(), boundary.cend());

                    if ((data.size() > chunksize + end_boundary.size()) || (b != data.cend()))  // got something to
                                                                                                // write
                    {
                        while (std::distance(data.cbegin(), b - LEN_OF_CRLF) > chunksize)
                        {
                            form_data.form_data(id,
                            filename,
                            data.cbegin(),
                            data.cbegin() + chunksize,
                            first_part,
                            false);
                            data.erase(data.begin(), data.begin() + chunksize);
                            first_part = false;
                            b = std::search(data.cbegin(), data.cend(), boundary.cbegin(), boundary.cend());
                        }

                        if (b != data.cend())
                        {
                            form_data.form_data(id, filename, data.cbegin(), b - LEN_OF_CRLF, first_part, true);  // first_b-2
                                                                                                                  // to
                            // avoid additional
                            // crlf in file end
                            parse_status = ParseStatus::Headers;
                            data.erase(data.begin(), b - LEN_OF_CRLF);
                            first_part = false;

                            if (std::distance(data.cbegin(), find_end_boundary()) <= 2 * LEN_OF_CRLF)
                            {
                                end_of_transmission = true;
                                data.clear();
                            }
                        }
                    }
                    else
                    {
                        get_more_data = true;
                    }
                }
            }  // while(!end_of_transmission && !get_more_data)

            end_of_transmission = false;
        }
        else if (mode == Mode::FormURLEncoded)
        {
            // URL encoded data can't be parsed in chunks, so wait until all data is received
            if (data.size() >= expected_content_length)
            {
                // Split data on '&' as it comes in. Each part is then expected to contain X=Y, so split on '='.
                // If a part doesn't contain a '=', put it back in the buffer to be used next time.
                // The way this works is that split() places any leftovers last in the returned vector
                // which means that it will be encountered last, thus the loops end at the same time.

                auto parts = split(data, std::vector<uint8_t>{ '&' });

                if (!parts.empty())
                {
                    data.clear();
                }

                URLEncoding encoding{};

                for (const auto& part : parts)
                {
                    auto equal_sign = std::find(part.cbegin(), part.cend(), '=');

                    if (equal_sign == part.cend())
                    {
                        std::copy(std::make_move_iterator(part.begin()),
                                  std::make_move_iterator(part.end()),
                                  std::back_inserter(data));
                    }
                    else
                    {
                        auto key_value = split(part, std::vector<uint8_t>{ '=' });

                        if (!key_value.empty())
                        {
                            std::string key{ key_value[0].begin(), key_value[0].end() };
                            auto key_res = encoding.decode(key, key.begin(), key.end());

                            std::string value{ key_value[1].begin(), key_value[1].end() };
                            auto value_res = encoding.decode(value, value.begin(), value.end());

                            if (key_res && value_res)
                            {
                                form_url_encoded_data.emplace(key, value);
                            }
                        }
                    }
                }

                // Perform the callback to the response handler with the parsed and decoded data.
                url_data.url_encoded(form_url_encoded_data);
            }
        }
    }

    BoundaryIterator MIMEParser::get_end_of_boundary(BoundaryIterator begin)
    {
        // Adjust for CRLF at beginning of boundary pattern
        auto offset = is_crlf(begin) ? LEN_OF_CRLF : 0;

        return begin + static_cast<Boundaries::difference_type>(boundary.size()) + offset;
    }

    bool MIMEParser::is_crlf(MIMEParser::BoundaryIterator start) const
    {
        return *start == '\r' && *(start + 1) == '\n';
    }

    std::tuple<MIMEParser::BoundaryIterator,
               std::unordered_map<std::string, std::string>,
               std::unordered_map<std::string, std::string>> MIMEParser::consume_headers(
        MIMEParser::BoundaryIterator begin,
        MIMEParser::BoundaryIterator end) const
    {
        std::unordered_map<std::string, std::string> headers{};
        std::unordered_map<std::string, std::string> content_disp{};
        auto start_of_actual_content = end;

        auto end_of_headers = std::search(begin, end, crlf_double.begin(), crlf_double.end());

        if (end_of_headers != end)
        {
            start_of_actual_content = end_of_headers + static_cast<long>(crlf_double.size());

            std::stringstream ss;

            std::for_each(begin, end_of_headers, [&ss](auto& c) {
                              if (c != '\n')
                              {
                                  ss << static_cast<char>(c);
                              }
            });

            std::string s;

            while (std::getline(ss, s, '\r'))
            {
                auto colon = std::find(s.begin(), s.end(), ':');

                if (colon != s.end() && !s.empty())
                {
                    if (std::distance(colon, s.end()) > 2)
                    {
                        headers[string_util::to_lower_copy({ s.begin(), colon })] = { colon + 2, s.end() };
                    }
                }
            }

            parse_content_disposition(headers, content_disp);
        }

        return std::make_tuple(start_of_actual_content, std::move(headers), std::move(content_disp));
    }

    void MIMEParser::parse_content_disposition(const std::unordered_map<std::string, std::string>& headers,
                                               std::unordered_map<std::string, std::string>& content_disposition) const
    {
        try
        {
            const auto& content_dis = headers.at(CONTENT_DISPOSITION);

            auto part = string_util::split(content_dis, ";", true);

            for (const auto& p : part)
            {
                auto key_value = string_util::split(p, "=", true);

                if (key_value.size() > 1)
                {
                    // Remove leading and ending quotation mark
                    auto filter = [](char c) { return c != '"'; };
                    content_disposition[key_value[0]] = string_util::trim(key_value[1], filter);
                }
                else
                {
                    // Single-value data
                    content_disposition[key_value[0]] = key_value[0];
                }
            }
        }
        catch (...)
        {
        }
    }
}
