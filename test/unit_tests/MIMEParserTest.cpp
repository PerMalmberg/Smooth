#include <catch.hpp>
#include <smooth/application/network/http/MIMEParser.h>
#include <smooth/core/filesystem/Path.h>
#include <smooth/core/filesystem/File.h>

using namespace smooth::core::filesystem;
using namespace smooth::application::network::http;

//const std::string post_data_result =

SCENARIO("MIMEParser")
{
    // Setup file system locks.
    FSLock::init(5);

    GIVEN("A mimeparser")
    {
        MIMEParser mime;

        WHEN("Provided with chunks of content data")
        {
            REQUIRE(mime.find_boundary(
                    "multipart/form-data; boundary=---------------------------8819839691792623414370909194"));

            const auto root =  Path{__FILE__}.parent();
            auto file = root / "test_data" / "post_result_data.txt";
            File f{file};
            std::vector<uint8_t> data;
            REQUIRE(f.read(data));

            THEN("Finds two files")
            {
                int count = 0;

                auto cb = [&count, &root](const std::string& name,
                                   const MIMEParser::BoundaryIterator& begin,
                                   const MIMEParser::BoundaryIterator& end) {
                    count++;

                    auto output = Path{"/tmp"} / name;
                    File o{output};
                    MIMEParser::MimeData data{begin, end};
                    o.write(data.data(), data.size());
                };

                mime.parse(data.data(), data.size(), cb);

                for (const auto& c : data)
                {
                    //mime.parse(&c, 1, cb);
                }

                REQUIRE(count == 3);
            }
        }
    }
}