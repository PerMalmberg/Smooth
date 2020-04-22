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

#include "http_server_test.h"
#include <memory>
#include "smooth/core/task_priorities.h"
#include "smooth/core/network/IPv4.h"
#include "smooth/core/filesystem/filesystem.h"
#include "smooth/core/filesystem/FSLock.h"
#include "smooth/application/network/http/regular/responses/StringResponse.h"
#include "smooth/application/network/http/regular/MIMEParser.h"
#include "smooth/core/SystemStatistics.h"
#include "WSEchoServer.h"
#include "wifi_creds.h"
#include "BlobResponder.h"
#include "UploadResponder.h"

using namespace std;
using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::filesystem;
using namespace smooth::core::network;
using namespace smooth::core::network::event;
using namespace smooth::core::logging;
using namespace smooth::application::network::http;
using namespace smooth::application::network::http::responses;

namespace http_server_test
{
    const char* private_key_data =
        "-----BEGIN RSA PRIVATE KEY-----\n"
        "MIIEowIBAAKCAQEAwIzeeGHJmTsFu1moTY65YPU0EKj9+QQ8Co+eozEPdyD+Yhes\n"
        "aGJLIvd5D+BIOLBPY8y6P8HjgNfUkzPQ0gOga2YZi01YsB909id/4mVNUNkDBQ3m\n"
        "cBb0V3rGCHX3SS01qgl8ucA/RPDLlPB1vYJN+fwoQ31x9spWlZd3S/ua0xARANG9\n"
        "mjgNKoa36b3cxBYfJB1cNu1MHh7oe2tpKO31sKVlzBvyrrDwXVRIYT0uTaJZ5yw5\n"
        "6CI98/TEiQIuXDXcSWiR1ar3wLdmNVYsvgj9qBEbMCQHAKEPDJ+9dna9hGKXCH6p\n"
        "Y0jPZoI/cdaACqkKyRw/ourxkopU6ZAjHTjnfwIDAQABAoIBAFw+IAXDsvR5VhGe\n"
        "rgYZra1JSpwAmUrGQI0+qFy9kA8d+z+635lSHXSu4Yw9gdvGewHwffk1RWvf+bJb\n"
        "TFqP6jYJP4uAjlAxgbK60YT/yr5EeDHfqtlf5cZSxrQe70xbIC5JRTsnYigAw0ai\n"
        "u4UB+6Ear59FZWc5H6miWkBIVK9vWOYt7sbR7QFwOd7yg6JhgPTAOsBumTx/Dul1\n"
        "jE8DfK/YferwMF+lP6otWGee8qOAFCKjKK5q1sWYeLMTcXNlN+2fd2Jrh2EsayJv\n"
        "oiRHrpzVXNMaxsPMq6DcsaDDy6XHp+wx3JOKVbRmVxtu42FgoLxUIGGJKyO3/tsS\n"
        "phWmkMECgYEA+FpNt96a/T2ZqeciACQkj+n2xctDYP2//z7AsC1zXS9I4jrsFMgL\n"
        "yxwtgy0aQQ4Uo+qDvHnt09ZT4SEJyhNuiMxQpcJqB3HFTWcZ2/36vQQtjdWyFKoP\n"
        "iWt8NFpFNPKP2r9pEQu/OJEEsYjXt+X4oQLNQiDDP5/+tMY7FKvE5AcCgYEAxnqw\n"
        "maN45JWi9SnwrIrES15Hd9q1e8BhbWQpIFYvQY65Y4sSGCdftmYQaFbP5s2Sinz5\n"
        "SM8DBldMjuca1Fxsj/eq+lAXNWpHe2l2oKBnVbSnTC74w61D7yDkPbDgYehCoDRk\n"
        "OZ1gWlegW/YOzkXb8BP1lcb2HSXTAAkY1qsDsskCgYAyWOxEiEZXbHOIdCj6allX\n"
        "K5ExjAF+2jJgJnKdxLkMfReDdXqCxANQC8hWPYCDC723dUV7oiRKu7qJe08mvkW8\n"
        "Sd9Xm1AvbisXg/1EEfsQ8pXhmR7e/ZLkwvuNCXfaQgpTyFYedIkxiISxERLaXy7G\n"
        "RXEjJM0wStkyALXdfO0PZQKBgHzSrD3zkRQxB0soFTRuEQVVRZ0a7wfk9WDWnQa5\n"
        "LP8Nw4nW7IpEOO19QtRbxGRi5MYz0pL3ne9e98bnkPf9KElieSTzOaVeMZZrvS9H\n"
        "sbU06AIBiHGHhjHdfa1Oj88DQuN+D4osb2xhr3gmRylyxd82rjyD5ERwhUKAdh6L\n"
        "bsgBAoGBALQCQeW6jdvVQIzlh3+im52ds+AWo+9re/+FctxyINlD8/iYjlCnRcV3\n"
        "53GaUpCspmSOKfDBzLrVlbdSNOl903Sf8zC2X0pqpN+CriV4gWyvIHPfEb+ZD2Mk\n"
        "OXwkD5lfchF5V+EBlLQu9f++MDtXtouoCUqeBpRil2ZSx2NhBUDq\n"
        "-----END RSA PRIVATE KEY-----\n";

    const char* server_cert_data =
        "-----BEGIN CERTIFICATE-----\n"
        "MIID/jCCAeYCFDRAbCBUb0xdLkG/sSTD+hzNXG71MA0GCSqGSIb3DQEBCwUAMFYx\n"
        "CzAJBgNVBAYTAlNFMRQwEgYDVQQHDAtBc3Rlcm9pZCA1MTEdMBsGA1UECgwUU2Vj\n"
        "cmV0IFVuaXZlcnNlIEluYy4xEjAQBgNVBAMMCWxvY2FsaG9zdDAeFw0xOTA0MTYx\n"
        "ODEzMTZaFw0yOTA0MTMxODEzMTZaMCExCzAJBgNVBAYTAlNFMRIwEAYDVQQDDAls\n"
        "b2NhbGhvc3QwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDAjN54YcmZ\n"
        "OwW7WahNjrlg9TQQqP35BDwKj56jMQ93IP5iF6xoYksi93kP4Eg4sE9jzLo/weOA\n"
        "19STM9DSA6BrZhmLTViwH3T2J3/iZU1Q2QMFDeZwFvRXesYIdfdJLTWqCXy5wD9E\n"
        "8MuU8HW9gk35/ChDfXH2ylaVl3dL+5rTEBEA0b2aOA0qhrfpvdzEFh8kHVw27Uwe\n"
        "Huh7a2ko7fWwpWXMG/KusPBdVEhhPS5NolnnLDnoIj3z9MSJAi5cNdxJaJHVqvfA\n"
        "t2Y1Viy+CP2oERswJAcAoQ8Mn712dr2EYpcIfqljSM9mgj9x1oAKqQrJHD+i6vGS\n"
        "ilTpkCMdOOd/AgMBAAEwDQYJKoZIhvcNAQELBQADggIBAEUYLx0jSQT79+wdvwPh\n"
        "O9xDyEIUc2Ih4XV19FUw3TrhBV92q+1M81lv60lTOl5fepvFG7XInav5hHs61Wx/\n"
        "IoNYWpCinapbU3E+2R6FOcTP1iTzfbeZwjcaIfXQ65ilX6j0T0oYAsfg8Y63kYKe\n"
        "miqPkkFNY0upcnyir/Z220urYm9+mT4HOPT4mNqMgXmLMlPffK8c1Fr2NXQMvsVz\n"
        "nEhxGYVQmqCbxceVkJD6swATtspc66GirBkAjuPfJOvCDfAHkzA3BZ26xziQdY8T\n"
        "UWCBtu4r5mhiWpU46L8f1CtG4RmRoi8g3STA7DaMM44Kl30lyBGT0vRfLyPU6vb3\n"
        "TCI4tdx5PFb46p1jeLL0IRShZ9pn/MPAja1+GE4k5/6WgQ2vVxf/90lBc7CbMrQ9\n"
        "CBMffs2HQKBwIYaPDtqoh9WHYZDv5TcVbfFrwtHNxRHWWofCxilSpjfjsoDwWdT/\n"
        "Imd+LMPdgUmA+dErjErrQ2uXLlrY2TVplxBaXUbeO+qMyrCNZIe1Ej5A99wyi2vW\n"
        "ydLYfWh34mAYiJEidqedt0TbbdwEAB8uO2XS0LSq4FBfPmXxhKgCSc99FxScVGi/\n"
        "xW1bZVeeORuAPAzGV2lMfGqIst59QXq7G5O4v2SgRyJ3tm+2scWyzfNwRqa43r9s\n"
        "s/WzJmP8eZyppmRvbTiYdBcy\n"
        "-----END CERTIFICATE-----\n";

    const char* ca_chain_data =
        "-----BEGIN CERTIFICATE-----\n"
        "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\n"
        "-----END CERTIFICATE-----\n";

    void fill(const char* src, std::vector<unsigned char>& target)
    {
        for (size_t i = 0; i < strlen(src); ++i)
        {
            target.push_back(static_cast<unsigned char>(src[i]));
        }

        // the mbedtls_x509_crt_parse function wants the size of the buffer, including the terminating 0 so we
        // add that too.
        target.push_back('\0');
    }

    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds(5000))
    {
    }

    void App::tick()
    {
        smooth::core::SystemStatistics::instance().dump();
    }

    void App::init()
    {
        std::stringstream ss;

        const int max_client_count = 6;
        const int listen_backlog = 6;

        Application::init();

        Log::info("App::Init", "Starting wifi...");
        network::Wifi& wifi = get_wifi();
        wifi.set_host_name("Smooth-ESP");
        wifi.set_auto_connect(true);
        wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
        wifi.connect_to_ap();

#ifdef ESP_PLATFORM
        Path web_root(SDCardMount::instance().mount_point() / "web_root");

        // Setup SD Card as per WROOVER Kit 3.x
        sd_card = std::make_unique<smooth::core::filesystem::MMCSDCard>(GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_4,
                                                                        GPIO_NUM_12, GPIO_NUM_13);

        sd_card->init(SDCardMount::instance(), false, 5);
#else
        const smooth::core::filesystem::Path test_path(__FILE__);
        const smooth::core::filesystem::Path web_root = test_path.parent() / "web_root";
#endif

        auto template_data_retriever = std::make_shared<DataRetriever>();
        template_data_retriever->add("{{title}}", "Smooth - a C++ framework for building apps on ESP-IDF");
        template_data_retriever->add("{{message}}",
                                    "Congratulations, you're browsing a web page on your ESP32 via Smooth framework by");
        template_data_retriever->add("{{github_url}}", "https://github.com/PerMalmberg");
        template_data_retriever->add("{{author}}", "Per Malmberg");
        template_data_retriever
        ->add("{{from_template}}", "This, and other text on this page are replaced on the fly from a template.");

        FSLock::set_limit(5);

        HTTPServerConfig cfg{ web_root, { "index.html" }, { ".html" }, template_data_retriever, MaxHeaderSize,
                              ContentChunkSize, MaxResponses };

        insecure_server = std::make_unique<InsecureServer>(*this, cfg);

        insecure_server->start(max_client_count, listen_backlog, std::make_shared<IPv4>("0.0.0.0", 8080));

        std::vector<uint8_t> ca_chain{};
        std::vector<uint8_t> own_cert{};
        std::vector<uint8_t> private_key{};
        std::vector<uint8_t> password{};

        fill(ca_chain_data, ca_chain);
        fill(server_cert_data, own_cert);
        fill(private_key_data, private_key);

        secure_server = std::make_unique<SecureServer>(*this, cfg);

        secure_server->start(max_client_count,
                             listen_backlog,
                             std::make_shared<IPv4>("0.0.0.0", 8443),
                             ca_chain,
                             own_cert,
                             private_key,
                             password);

        // Different request handler are created for each of the two servers. Although the
        // server are running on the same thread, a handler might not be state-less so
        // if requests are received on both servers at the same time, there is no guarantee
        // in which order the data is processed between the two handlers.

        secure_server->on(HTTPMethod::GET, "/api/blob", std::make_shared<BlobResponder>());
        secure_server->on(HTTPMethod::POST, "/upload", std::make_shared<UploadResponder>());
        secure_server->enable_websocket_on<WSEchoServer>("/echo");

        insecure_server->on(HTTPMethod::GET, "/api/blob", std::make_shared<BlobResponder>());
        insecure_server->on(HTTPMethod::POST, "/upload", std::make_shared<UploadResponder>());
        insecure_server->enable_websocket_on<WSEchoServer>("/echo");
    }
}
