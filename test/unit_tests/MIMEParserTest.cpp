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

#include <catch.hpp>
#include <smooth/application/network/http/regular/MIMEParser.h>
#include <smooth/core/filesystem/Path.h>
#include <smooth/core/filesystem/File.h>
#include <smooth/application/hash/sha.h>
#include <smooth/core/filesystem/Fileinfo.h>

using namespace smooth::core::filesystem;
using namespace smooth::application::network::http;
using namespace smooth::application::network::http::regular;

SCENARIO("MIMEParser - multipart/form-data - Text files")
{
    // Setup file system locks.
    FSLock::init(5);

    GIVEN("A mimeparser")
    {
        MIMEParser mime;

        WHEN("Provided with chunks of content data")
        {
            REQUIRE(mime.detect_mode(
                    "multipart/form-data; boundary=---------------------------8819839691792623414370909194", 0));

            const auto root = Path{__FILE__}.parent();
            auto file = root / "test_data" / "post_result_data.txt";
            File f{file};
            std::vector<uint8_t> data;
            REQUIRE(f.read(data));

            THEN("Finds two files")
            {
                int count = 0;

                auto form_data = [&count, &root](const std::string& name,
                                          const MIMEParser::BoundaryIterator& begin,
                                          const MIMEParser::BoundaryIterator& end) {
                    count++;


                    constexpr auto text_txt = std::array<uint8_t, 32>{0x7b, 0x6f, 0x8a, 0x3d, 0x64, 0xdf, 0x0d, 0x8a,
                                                                      0xe3, 0xb1, 0x4c, 0x36, 0x1d, 0xc2, 0xa9, 0x3a,
                                                                      0x96, 0xef, 0x37, 0x76, 0xdf, 0x76, 0xfb, 0xab,
                                                                      0x2a, 0xbb, 0x23, 0xb8, 0xf5, 0x10, 0x71, 0x2a};

                    constexpr auto text2_txt = std::array<uint8_t, 32>{0xb2, 0x9e, 0x68, 0x2f, 0x6b, 0x24, 0xb9, 0xfc,
                                                                       0x7c, 0xf0, 0x89, 0x09, 0x8c, 0x63, 0x5c, 0xc6,
                                                                       0xd2, 0x95, 0x1e, 0x19, 0x16, 0x16, 0xcd, 0x97,
                                                                       0x2c, 0x5d, 0x49, 0x00, 0x4d, 0x45, 0x65, 0xe6};

                    MIMEParser::MimeData data{begin, end};
                    auto hash = smooth::application::hash::sha256(data.data(), data.size());

                    if (name == "file_to_upload")
                    {
                        REQUIRE(hash == text_txt);
                    }
                    else if (name == "second_file_to_upload")
                    {
                        REQUIRE(hash == text2_txt);
                    }
                };

                auto url_encoded_data = [](const std::unordered_map<std::string, std::string>&){};

                // Give the parser data in as small chunks as possible, i.e. one byte at a time
                // to make sure we can handle such split data.
                for (const auto& c : data)
                {
                    mime.parse(&c, 1, form_data, url_encoded_data);
                }

                REQUIRE(count == 3);
            }
        }
    }
}

SCENARIO("MIMEParser - multipart/form-data - Binary files")
{
    // Setup file system locks.
    FSLock::init(5);

    GIVEN("A mimeparser")
    {
        MIMEParser mime;

        WHEN("Provided with chunks of content binary data")
        {
            REQUIRE(mime.detect_mode(
                    "multipart/form-data; boundary=---------------------------7184603361412956941791020073", 0));

            const auto root = Path{__FILE__}.parent();
            auto file = root / "test_data" / "post_binary_data.bin";
            File f{file};
            std::vector<uint8_t> data;
            REQUIRE(f.read(data));

            THEN("Finds two PNG files")
            {
                int count = 0;

                auto form_data = [&count, &root](const std::string& name,
                                          const MIMEParser::BoundaryIterator& begin,
                                          const MIMEParser::BoundaryIterator& end) {

                    constexpr auto accept_png = std::array<uint8_t, 32>{0x0a, 0x73, 0x3b, 0x99, 0xfc, 0xd0, 0x3c, 0x5e,
                                                                        0x63, 0x59, 0xd0, 0x97, 0x3a, 0x16, 0x9b, 0xbf,
                                                                        0xaf, 0x94, 0x48, 0x52, 0x27, 0x43, 0x74, 0x80,
                                                                        0xd9, 0xc7, 0x03, 0xbb, 0xe5, 0x8e, 0x4b, 0x4c};

                    constexpr auto anchor_png = std::array<uint8_t, 32>{0xc6, 0xbe, 0x60, 0xaf, 0x8a, 0xf7, 0xb9, 0x83,
                                                                        0x0c, 0xdc, 0xb0, 0x26, 0x84, 0xa3, 0x84, 0x4a,
                                                                        0x99, 0x88, 0x92, 0x6c, 0x3d, 0x1f, 0x3f, 0x5c,
                                                                        0xb6, 0xcd, 0x00, 0xe2, 0x72, 0x60, 0x76, 0x78};

                    count++;

                    MIMEParser::MimeData data{begin, end};

                    auto hash = smooth::application::hash::sha256(data.data(), data.size());

                    if (name == "file_to_upload")
                    {
                        REQUIRE(hash == accept_png);
                    }
                    else if (name == "second_file_to_upload")
                    {
                        REQUIRE(hash == anchor_png);
                    }
                };

                auto url_encoded_data = [](std::unordered_map<std::string, std::string>&){};

                // Give the parser data in as small chunks as possible, i.e. one byte at a time
                // to make sure we can handle such split data.
                for (const auto& c : data)
                {
                    mime.parse(&c, 1, form_data, url_encoded_data);
                }

                REQUIRE(count == 3);
            }
        }
    }
}

SCENARIO("MIMEParser - application/x-www-form-urlencoded")
{
    // Setup file system locks.
    FSLock::init(5);

    GIVEN("A mimeparser")
    {
        MIMEParser mime;

        WHEN("Provided with chunks of url encoded data")
        {
            const auto root = Path{__FILE__}.parent();
            auto file = root / "test_data" / "url_encoded.txt";
            FileInfo info{file};

            REQUIRE(mime.detect_mode("application/x-www-form-urlencoded", info.size()));

            File f{file};
            std::vector<uint8_t> data;
            REQUIRE(f.read(data));

            THEN("Finds the url encoded data")
            {
                auto form_data = MIMEParser::FormDataCallback();

                std::unordered_map<std::string, std::string> received{};
                auto url_encoded_data = [&received](std::unordered_map<std::string, std::string>& url_data){
                    received = std::move(url_data);
                };

                // Give the parser data in as small chunks as possible, i.e. one byte at a time
                // to make sure we can handle such split data.
                for (const auto& c : data)
                {
                    mime.parse(&c, 1, form_data, url_encoded_data);
                }

                REQUIRE(received.at("free_text") == "test text");
                REQUIRE(received.at("submit") == "Send text");
            }
        }
    }
}