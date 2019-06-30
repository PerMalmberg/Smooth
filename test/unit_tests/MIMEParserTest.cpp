#include <catch.hpp>
#include <smooth/application/network/http/MIMEParser.h>

using namespace smooth::application::network::http;

const std::string two_files = R"!(
-----------------------------83469447617042511111590758916

Content-Disposition: form-data; name="file_to_upload"; filename="text.txt"

Content-Type: text/plain



This is a text file
with multiple lines.

Lets count to 10:
1
2
3
4
5
6
7
8
9
0
this is the last line of text.


-----------------------------83469447617042511111590758916

Content-Disposition: form-data; name="second_file_to_upload"; filename="text2.txt"

Content-Type: text/plain



This is the second file.
It has no exciting content.


-----------------------------83469447617042511111590758916

Content-Disposition: form-data; name="submit"



Upload file

-----------------------------83469447617042511111590758916--
)!";

SCENARIO("MIMEParser")
{
    GIVEN("A mimeparser")
    {
        MIMEParser mime;

        WHEN("Provided with chunks of content data")
        {
            REQUIRE(mime.find_boundary(
                    "multipart/form-data; boundary=-----------------------------83469447617042511111590758916"));

            THEN("Finds two files")
            {
                int count = 0;

                auto cb = [&count](std::string&& name, const std::vector<uint8_t>& content){count++;};

                //mime.parse(reinterpret_cast<const uint8_t*>(two_files.c_str()), two_files.length(), cb);

                for(const auto& c : two_files)
                {
                    if(mime.parse(reinterpret_cast<const uint8_t*>(&c), 1, cb))
                    {
                        ++count;
                    }
                }

                REQUIRE(count == 3);
            }
        }
    }
}