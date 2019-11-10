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

#include "secure_socket_test.h"
#include "smooth/core/Task.h"
#include "smooth/core/task_priorities.h"
#include "smooth/core/Application.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/network/BufferContainer.h"
#include "smooth/core/network/MbedTLSContext.h"
#include "smooth/application/network/http/HTTPPacket.h"
#include "smooth/application/network/http/regular/HTTPMethod.h"
#include "smooth/application/network/http/regular/ResponseCodes.h"
#include <cassert>
#include "wifi_creds.h"

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::timer;
using namespace smooth::core::network;
using namespace smooth::core::network::event;
using namespace smooth::core::logging;
using namespace smooth::application::network::http;

namespace secure_socket_test
{
    // Certificates extracted from ftp.sunet.se using the following commands, taken from
    // https://unix.stackexchange.com/a/487546/228535
    //
    // openssl s_client -showcerts -verify 5 -connect ftp.sunet.se:443 < /dev/null | awk '/BEGIN/,/END/{
    // if(/BEGIN/){a++}; out="cert"a".crt"; print >out}' && for cert in *.crt; do newname=$(openssl x509 -noout -subject
    // -in $cert | sed -n 's/^.*CN=\(.*\)$/\1/; s/[ ,.*]/_/g; s/__/_/g; s/^_//g;p').pem; mv $cert $newname; done

    constexpr const char* const ftp_sunet_se =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIHJjCCBg6gAwIBAgISA45221Cqa0hSbWgIHGcpuwLzMA0GCSqGSIb3DQEBCwUA\n"
        "MEoxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MSMwIQYDVQQD\n"
        "ExpMZXQncyBFbmNyeXB0IEF1dGhvcml0eSBYMzAeFw0xOTAyMjEyMjE4NDVaFw0x\n"
        "OTA1MjIyMjE4NDVaMBkxFzAVBgNVBAMTDmZ0cC5hY2MudW11LnNlMIIBIjANBgkq\n"
        "hkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwkYx09gtpoiTU+gwCwpi5zJxrDW0XlX3\n"
        "3LbrPrW+KwmzboYrSRspbFat/VInKfGhQ9BNEMidIWsk0nSRpqbF0pbPDXo8xJBl\n"
        "Zw6ltoeW2qC1n+Rs+56rFV559IFdvxCOwG3n0j36T0FLDpXr3K8kR9dc46EIBaXQ\n"
        "Fg32UyOHxaGSrxV6459+FZYcElPCEZqH2zXfXOehAPAxPzoTP0dwe71j17s066Ns\n"
        "LKW/aAEk+/NJtygYRBT+aioCEDTxt5vs8o9izrXg5qXBFbDj757VoWJI+nwtWxBp\n"
        "XGN0rPa1UtAiD2ccxwD9Cc4RR2lBunjp8DWlMAKCrJa4izNmTpow+QIDAQABo4IE\n"
        "NTCCBDEwDgYDVR0PAQH/BAQDAgWgMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEF\n"
        "BQcDAjAMBgNVHRMBAf8EAjAAMB0GA1UdDgQWBBThHrlBMlmiYNDQMHcwHVbAM0ET\n"
        "wzAfBgNVHSMEGDAWgBSoSmpjBH3duubRObemRWXv86jsoTBvBggrBgEFBQcBAQRj\n"
        "MGEwLgYIKwYBBQUHMAGGImh0dHA6Ly9vY3NwLmludC14My5sZXRzZW5jcnlwdC5v\n"
        "cmcwLwYIKwYBBQUHMAKGI2h0dHA6Ly9jZXJ0LmludC14My5sZXRzZW5jcnlwdC5v\n"
        "cmcvMIIB6QYDVR0RBIIB4DCCAdyCDmFjYy5kbC5vc2RuLmpwghBhcmNoaXZlLnN1\n"
        "bmV0LnNlghFjYWVzYXIuYWNjLnVtdS5zZYIVY2Flc2FyLmZ0cC5hY2MudW11LnNl\n"
        "ghJjZGltYWdlLmRlYmlhbi5vcmeCEGNsb3VkLmRlYmlhbi5vcmeCDmZ0cC5hY2Mu\n"
        "dW11LnNlgg1mdHAuZ25vbWUub3JnggxmdHAuc3VuZXQuc2WCEWdlbW1laS5hY2Mu\n"
        "dW11LnNlghVnZW1tZWkuZnRwLmFjYy51bXUuc2WCEWdlbnNoby5hY2MudW11LnNl\n"
        "ghVnZW5zaG8uZnRwLmFjYy51bXUuc2WCDmdldC5kZWJpYW4ub3JnghRoYW1tdXJh\n"
        "YmkuYWNjLnVtdS5zZYIYaGFtbXVyYWJpLmZ0cC5hY2MudW11LnNlghttZWV0aW5n\n"
        "cy1hcmNoaXZlLmRlYmlhbi5uZXSCE25hcG9sZW9uLmFjYy51bXUuc2WCF25hcG9s\n"
        "ZW9uLmZ0cC5hY2MudW11LnNlghFzYWltZWkuYWNjLnVtdS5zZYIVc2FpbWVpLmZ0\n"
        "cC5hY2MudW11LnNlghZ0dXRhbmtoYW1vbi5hY2MudW11LnNlghp0dXRhbmtoYW1v\n"
        "bi5mdHAuYWNjLnVtdS5zZTBMBgNVHSAERTBDMAgGBmeBDAECATA3BgsrBgEEAYLf\n"
        "EwEBATAoMCYGCCsGAQUFBwIBFhpodHRwOi8vY3BzLmxldHNlbmNyeXB0Lm9yZzCC\n"
        "AQQGCisGAQQB1nkCBAIEgfUEgfIA8AB3AOJpS64m6OlACeiGG7Y7g9Q+5/50iPuk\n"
        "jyiTAZ3d8dv+AAABaRJaq4wAAAQDAEgwRgIhAJ7O2IAg1W3J1p8Q+B92u9oWDRm1\n"
        "8HxyhwsJywGuSrheAiEA7FT+f54b1YWdSxTHpkxErlYg5Fv6bCZZ57Qt29q4KzIA\n"
        "dQApPFGWVMg5ZbqqUPxYB9S3b79Yeily3KTDDPTlRUf0eAAAAWkSWqmZAAAEAwBG\n"
        "MEQCIAkSsdwEMxHJZUXRCfeAgN890h2l8C6ZvwwMWOmzxEWIAiA6PC52+bhD9JF2\n"
        "IFTvEIiNghd/pvQR4PnwPBQYFwHfzjANBgkqhkiG9w0BAQsFAAOCAQEAd782J0lu\n"
        "cRFZCK7iC7QQgEHg/TzlIWdY6CWJ5t1liSI9oRgeusXnFeIgUdGe0fUKh1UPd5WW\n"
        "fPAB286/n/vdUp2qf0JOJbfwIDBAyA6MxYPyvOtOj8gFnxwnbvXafGniK7fFURe1\n"
        "EwelD/DrC/9fowl4zllkQeIeJl2zYvBFmVAJc8bIVhFD7Pa6M6loIhjBhoAKbqeg\n"
        "9phRIrFqLnCtvmnoRs7MX+GieuKoRNFWb3sAqwQ0czoYG+AY+XQONTH2vRSbmYfJ\n"
        "ZoeYnb5Oq6BX4wbls6fcuGUtPZiLib78cF/h4wZXnET4ubf+3hq5p98GK8JHWpSe\n"
        "IpbKMU1XOP06NA==\n"
        "-----END CERTIFICATE-----\n"

        "-----BEGIN CERTIFICATE-----\n"
        "MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\n"
        "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"
        "DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n"
        "SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\n"
        "GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\n"
        "AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\n"
        "q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\n"
        "SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\n"
        "Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\n"
        "a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\n"
        "/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\n"
        "AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\n"
        "CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\n"
        "bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\n"
        "c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\n"
        "VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\n"
        "ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\n"
        "MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\n"
        "Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\n"
        "AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\n"
        "uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\n"
        "wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\n"
        "X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\n"
        "PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\n"
        "KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\n"
        "-----END CERTIFICATE-----\n";

    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1)),
              net_status(NetworkStatusQueue::create(2, *this, *this))
    {
    }

    void App::init()
    {
        Application::init();

        Log::info("App::Init", "Starting wifi...");
        network::Wifi& wifi = get_wifi();
        wifi.set_host_name("Smooth-ESP");
        wifi.set_auto_connect(true);
        wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
        wifi.connect_to_ap();
    }

    void App::tick()
    {
    }

    std::unique_ptr<std::vector<unsigned char>> App::get_certs() const
    {
        auto ca_cert = std::make_unique<std::vector<unsigned char>>();

        for (size_t i = 0; i < strlen(ftp_sunet_se); ++i)
        {
            ca_cert->push_back(static_cast<unsigned char>(ftp_sunet_se[i]));
        }

        // the mbedtls_x509_crt_parse function wants the size of the buffer, including the terminating 0 so we
        // add that too.
        ca_cert->push_back('\0');

        return ca_cert;
    }

    void App::event(const smooth::core::network::NetworkStatus& event)
    {
        if (event.get_event() == NetworkEvent::GOT_IP)
        {
            if (!sock)
            {
                buff = std::make_shared<BufferContainer<Proto>>(*this, *this, *this, *this,
    std::make_unique<smooth::application::network::http::HTTPProtocol>(1024, 4096, *this));

                // If no certificates are provided, no certificate verification will be performed.
                auto ca_cert = get_certs();

                tls_context = std::make_unique<MBedTLSContext>();
                tls_context->init_client(*ca_cert);

                sock = SecureSocket<Proto>::create(buff, tls_context->create_context());
                sock->start(std::make_shared<IPv4>("ftp.sunet.se", 443));
            }
        }
    }

    void App::event(const TransmitBufferEmptyEvent&)
    {
    }

    void App::event(const DataAvailableEvent<Proto>& packet)
    {
        Proto::packet_type p;
        packet.get(p);

        if (!p.is_continuation())
        {
            // First packet
            assert(p.response_code() == ResponseCode::OK);
            received_content.insert(received_content.end(), p.data().begin(), p.data().end());
        }
        else
        {
            // Seconds and onwards
            received_content.insert(received_content.end(), p.data().begin(), p.data().end());
        }

        if (!p.is_continued())
        {
            // Last packet
            sock->stop("Last packet received");

            std::stringstream ss;

            for (auto c : received_content)
            {
                ss << static_cast<char>(c);
            }

            std::string s{ ss.str() };

            // Don't compare the actual signature as it changes when Debian releases a new version.
            bool has_begining = s.find("-----BEGIN PGP SIGNATURE-----") != std::string::npos;
            bool has_ending = s.find("-----END PGP SIGNATURE-----") != std::string::npos;
            assert(has_begining);
            assert(has_ending);
            Log::info("Result", "============> SUCCESS <============ ");
        }
    }

    void App::event(const ConnectionStatusEvent& ev)
    {
        if (ev.is_connected())
        {
            Log::info("Connection status: ", "{}", ev.is_connected());

            // *INDENT-OFF*
            sock->send(
                       HTTPPacket(
                           HTTPMethod::GET,
                           "/debian-cd/current-live/amd64/iso-hybrid/MD5SUMS.sign",
                           {
                               { "UserAgent", "Mozilla/4.0" },
                               { "Host", "ftp.sunet.se" }
                           },
                           {
                           }
                       )
                    );
            // *INDENT-ON*
        }
    }
}
