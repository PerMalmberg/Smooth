#include "server_socket_test.h"
#include <deque>
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/Application.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/core/network/ServerSocket.h>
#include <smooth/core/network/SecureServerSocket.h>
#include "wifi_creds.h"

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::network;
using namespace smooth::core::network::event;
using namespace smooth::core::logging;

const char* private_key_data = "-----BEGIN RSA PRIVATE KEY-----\n"
"MIIJKQIBAAKCAgEAxfYLiptWm3/zq0Q+xBkG0A9GoF1mFq3ck47df1ukKUwlhbqL\n"
"SBHOGVhSL8pKAZ/9DfOBgGzX4G76uWPcXdMxvkFX9yMIWcz6gpozUq2dOwGTcjE6\n"
"+HWJw1f1eM/KfFntv2/gYiElziKb5zsfhm+2pp6c6Lyy88PC8hQty1rUOSyLdIV2\n"
"lLCrEmGKZBSqDi9unWN+K8fOA3sBWmhx+nY6PwrrrEesB3gr3Fss/KdoaZk0abSy\n"
"xb+NOA5VshsPklhG4uy1xRSLs+NGTG12wNlfEH5qjrEz7DXq0kswyvxMp+aXfPDF\n"
"JXUdJ6wrXu1qui4w3F8FqQO6TFfYjjua/vE5Pw8WYQi3fsc9iKc/BDAWl3/bMPII\n"
"7mOFbiBRhDiDdrwvS4vSzzgD0n9gkjHf0wufqseWp5+cx2HFWbYMRQpRhMDyp0na\n"
"mzCBaZIoP4FZsOARvWN27REiRIJUS+bYu3rDCP62HY81Vkytiswzp+bLL+k8WTP8\n"
"F+Trm1Xe/HFC5d+YGRotOlY6MuWlS2/1te3d7/7cZ7bBfUSpl9bD7xLGmuE7sNwj\n"
"phgWOTIyqLc4EiYdRuKzPiBONFtWJ2yeg8PeAT0RFaOlPZgBYiNkEYbyQWiTWcTu\n"
"K5gRuZ4SRdUk+8JMwSD6ZUkpr77O/gn0iGhl095xnT0IKRzuqtamFxzQbsECAwEA\n"
"AQKCAgADLXvp8axmOpuSuDm5tB0j4drkYBtvacMMq1Jq5PGxqANCLkhmE8MXLQ/E\n"
"rJ6bgBHz02DUyup8M8vahfvXlbhKsYnT8cAEkQHEbu07+oV3mQLg5mFn85sOg67x\n"
"DYTWkir607TqXlXZBT0zbQOIiOqJqioiokTNHOtYL3K8vOyDLm/eovXQVTWpyd8K\n"
"1DoWmDRSWnKACWkJLURMSzjpKFSvBmHaFhxw0kEB/ISwWY+XgloRCFGGUxjVOYWu\n"
"ojiRxND+e4Y/b6D+djt+ppMtbU/VV/Il8DpBJGxsi8BwwbQJTe5IlyTTARpCtMTN\n"
"dUotSuiF4RCaMfEfoJldTx+h0AV3MmPxr5EnIlV7b/7wYHYIkYqgzi6+kXij3Pth\n"
"YXxLv9+KTKTAvwqiLBJgxwjHM2gsfOTRW9XNoet+QCM3wkBmGMTrApkEiaCT5tds\n"
"n2NKcByH4D5OXy2+f0paH06fBBQJYekiEObGTct6MpxL8TW2DpDeOmQD8JFPvLyy\n"
"3EwRYgLNZgprB2meHKlmQ61I/aGCNSvc3KxRHRzb9Xvgd4Z6rew+sBfdCLnXDcTM\n"
"4+oVYCmGKP6Gzw7RPmQq6TjJkb37pp+8ngXIpkP7L9+2m/rzSmAnPT+HQYi7DEib\n"
"qZspHt002PXKzAKQ/+D5MSldbqyeGUpfbd1ZJHLTkUNSpWjwAQKCAQEA/Q1Fygkq\n"
"EdIPBF5UW9lxhsv20CEKfl9p6yNdU7kOhK1f8c9+S/6VzVCnbCBpdW9wvTZuvnMz\n"
"944p/OZjjaoUQl25EXQCWQPNAzepGCmUGLvGivxAkZ0PQnpypAAFp8PdnBfFzl5T\n"
"4QOBYlJlsd2SziVLeyM+F1uS1vmZzV9nDVHhu2ZjOKf/hxTGiN6H1zo7NK34mnMU\n"
"b7rDyOSHHEI/FgjrhhQLqcL0TrZ2HBS+3Idg45KXRu7VcwVszoILLdfxeFCyjUWh\n"
"eg5PXb6Am16oeqpczzaaE7ePK7sDkW+WKnQ0AGkXEtzWoW98NDxeiB7hnKjtF/yf\n"
"7orUf3KTp+wn4QKCAQEAyER23KbcIkny8xcbDBvNf5T7o3m3ESS345mfYfL9GnqT\n"
"88t+uJUAqSr3FS/H9btyPKySRZ/4ttbHqTdtS/JOJEv6/f2ulgCldcu6txRp0ktC\n"
"lMjhrESOjIqpKVuNQjFuEymK2PhMQlk/aj8OQDTZY2HPmxmAHBot9Yr1YPxlAezP\n"
"A1K/Tw5sPo/OnjJFtqO+2Ec/SMuKjWVSBiAl6xfGxKV7RtbfZda6w2Yi26GNJN1d\n"
"HCl1y1l1AE53HbKqliHzZIkWvDzkIqm3TylXkKwcA3W8YB2mrEOi5kHbhTYxgtas\n"
"uL8dGrazh/cYMJhiUqM6y+PEhyrtAKYGA8KfgaWi4QKCAQADqg6Jp2pRib4n7QnB\n"
"rrtjHU78oo3ZWJ5bBk8SqEpah3wtLzj/9rbKm1FhUnLUfYGfa8tcvKRTAF8BR4DH\n"
"i1/xuhO0+f4StehhFIxBSlDXrthoKNDyitiuuMEyOMHcNQzfdsSpgVorlvZnqxPH\n"
"6JJ4o7edF9yNUAknTcP5YllGqxnesdPb/SrJFPbvAnHrHu++iopgCiyQlZ7CKPT/\n"
"5sGtGwH2FhI5KhaJkUDSlUZSQS+Y/AVuxISYvspdjyqERm07tcdwHkMBVnW1wM1x\n"
"8oP0CsV/RqYihPTwyCt8DqaZBMe//cWUw1fkTI3636jqxa+0qgmM4F6cgKwlEE6t\n"
"DtFhAoIBAQC97G/tAjFyMz88WoVEDnnsrJWYiy2aDIB4UJBei9qM6gK2Ch67XlOq\n"
"Gx0H8rBn0sSb1e1CKlvsyfO//tigVcLVmVkG1zZi+2nvNK52Qd25mbYi8/9RKBMo\n"
"JbgXiMOGKuqBMbyD17Y6lfkWAYa6TWR8Y55HMP/Vpt4/JvFecP45vSL1u9vCVaNC\n"
"EY+ZZ4PFmbS715Wkq6NSROxs8sADHCFdDLLEk7J5GhM0jAAy2hrJA0XyFz+nAcd7\n"
"6u38NByjFL+GcJwnGHgih2FYqZRpl09Z2KY1YOlYJSyvwXU1+BcsRfq3mkc3tly8\n"
"bRF7xIg6ZqK0CeWvan4pR0oc9s3eXf8BAoIBAQDidATMiWLssBhTFpTQ4cAzxtiu\n"
"AQu6sUkzIcEAmAaJTgbZsb0OldQDO/Lb2kzN8L7avKj1VW57VRkYfIqSXTA9t/IO\n"
"Y8fZ+W3bAEuXpPMwFUbRX67wykelUpDnEYe1Ayl9SVCtcadYsNafWx9v02U1nC1m\n"
"4Gb2SEJWm7B8Lb2K3aRthw8ga4G9ELp/Se5rU/v5LTLatt1WxNynQ6rLVL4HdPIa\n"
"hsdoUdCrYMJwKDk/ERNVrSh7rdwCo1AKf0RMYI46jzPJVyvfUWhVQRHobswFrEmU\n"
"cmGJS5++LoV5LlRa2RDoMNvs4z2hh6Q1hDLN+WQt5kQNbjytGM2NVhd5xOet\n"
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


namespace server_socket_test
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds(1000))

    {
    }

    void App::init()
    {
        Application::init();
#ifdef ESP_PLATFORM
        Log::info("App::Init", Format("Starting wifi..."));
        network::Wifi& wifi = get_wifi();
        wifi.set_host_name("Smooth-ESP");
        wifi.set_auto_connect(true);
        wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
        wifi.connect_to_ap();
#endif

        // The server creates StreamingClients which are self-sufficient and never seen by the main
        // application (unless the implementor adds such bindings).
        //server = ServerSocket<StreamingClient, StreamingProtocol>::create(*this, 5);
        //server->start(std::make_shared<IPv4>("0.0.0.0", 8080));

        // Point your browser to http://localhost:8080 and watch the output.
        // Or, if you're on linux, do "echo ` date` | nc localhost 8080 -w1"



        std::vector<unsigned char> ca_chain{};
        std::vector<unsigned char> own_certs{};
        std::vector<unsigned char> private_key{};
        std::vector<unsigned char> password{};

        fill(ca_chain_data, ca_chain);
        fill(private_key_data, private_key);
        fill(server_cert_data, own_certs);

        server = SecureServerSocket<StreamingClient, StreamingProtocol>::create(*this, 5, ca_chain, own_certs, private_key, password);
        server->start(std::make_shared<IPv4>("0.0.0.0", 8443));
    }

}

