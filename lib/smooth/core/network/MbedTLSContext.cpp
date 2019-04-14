#include <smooth/core/network/MbedTLSContext.h>
#include <mbedtls/error.h>
#include <memory>
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        namespace network
        {

            const char SSL_CA_PEM[] =
                    /* GlobalSign Root certificate */
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
                    "-----END CERTIFICATE-----\n";

            // Things to improve:
            // https://tls.mbed.org/discussions/generic/one-time-and-per-session-initialization-functions

            static constexpr const char* tag = "MBedTLSContext";
#ifndef ESP_PLATFORM
            static void mbedtls_debug(void* /*ctx*/, int /*level*/, const char* /*file*/, int /*line*/, const char* str)
            {
                Log::debug(tag, str);
            }
#endif

            bool MBedTLSContext::init_client(const std::vector<unsigned char>& ca_certificates)
            {
                mbedtls_ssl_init(&ssl);
                mbedtls_ssl_config_init(&conf);
                mbedtls_x509_crt_init(&ca_cert);
                mbedtls_entropy_init(&entropy);
                mbedtls_ctr_drbg_init(&ctr_drbg);
#ifndef ESP_PLATFORM
                mbedtls_ssl_conf_dbg(&conf, mbedtls_debug, stdout);
                mbedtls_debug_set_threshold(1);
#endif
                auto res = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, nullptr, 0);

                if (res != 0)
                {
                    Log::error(tag, Format("mbedtls_ctr_drbg_seed returned {1}", Int32(res)));
                }
                else
                {
                    res = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                                      MBEDTLS_SSL_PRESET_DEFAULT);
                    if (res != 0)
                    {
                        Log::error("MBedTLSContext", Format("mbedtls_ssl_config_defaults returned {1}", Int32(res)));
                    }
                    else
                    {
                        if (ca_certificates.empty())
                        {
                            mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
                        }
                        else
                        {
                            mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);

                            res = mbedtls_x509_crt_parse(&ca_cert, ca_certificates.data(), ca_certificates.size() + 1);

                            if(res < 0)
                            {
                                char buf[128];
                                mbedtls_strerror(res, buf, sizeof(buf));
                                Log::error("MBedTLSContext", Format("mbedtls_x509_crt_parse returned {1}: {2}", Int32(res), Str(buf)));
                            }
                            else if(res > 0)
                            {
                                Log::error("MBedTLSContext", Format("mbedtls_x509_crt_parse failed to parse {1} certificates.", Int32(res)));
                            }
                            else
                            {
                                mbedtls_ssl_conf_ca_chain(&conf, &ca_cert, nullptr);
                            }
                        }

                        if(res == 0)
                        {
                            mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
                            res = mbedtls_ssl_setup(&ssl, &conf);

                            if (res != 0)
                            {
                                Log::error("MBedTLSContext", Format("mbedtls_ssl_setup returned {1}", Int32(res)));
                            }
                        }
                    }
                }

                return res == 0;
            }

            MBedTLSContext::~MBedTLSContext()
            {
                mbedtls_ctr_drbg_free(&ctr_drbg);
                mbedtls_entropy_free(&entropy);
                mbedtls_x509_crt_free(&ca_cert);
                mbedtls_ssl_config_free(&conf);
                mbedtls_ssl_free(&ssl);
            }
        }
    }
}