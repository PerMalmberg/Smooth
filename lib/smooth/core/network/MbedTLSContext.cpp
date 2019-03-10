#include <smooth/core/network/MbedTLSContext.h>
#include <memory>
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        namespace network
        {
            // Things to improve:
            // https://tls.mbed.org/discussions/generic/one-time-and-per-session-initialization-functions

            constexpr const char* tag = "MBedTLSContext";
#ifndef ESP_PLATFORM
            static void mbedtls_debug(void* ctx, int level, const char* file, int line, const char* str)
            {
                (void) ctx;
                (void) level;
                (void) file;
                (void) line;

                Log::debug(tag, str);
            }
#endif
            /*
            static int verify(void* data, mbedtls_x509_crt* crt, int depth, uint32_t* flags)
            {
                // https://tls.mbed.org/api/group__x509__module.html#ga98ed4504e4f832b735a230acf54fcde3
                const uint32_t buff_size = 1024;
                auto buffer = std::make_unique<char[]>(buff_size);

                (void) data;

                Log::debug(tag, Format("Verifying certificate, depth: {1}", Int32(depth)));

                mbedtls_x509_crt_info(buffer.get(), buff_size - 1, "  ", crt);
                Log::debug(tag, buffer.get());

                if (*flags == 0)
                {
                    Log::debug(tag, "Certificate verified");
                }
                else
                {
                    mbedtls_x509_crt_verify_info(buffer.get(), buff_size, "", *flags);
                    Log::debug(tag, buffer.get());
                }

                return 0;
            }
            */

            bool MBedTLSContext::init()
            {
                mbedtls_net_init(&connection_identifier);
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
                        // TODO: Implement auth: MBEDTLS_SSL_VERIFY_REQUIRED
                        mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);

                        mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
                        mbedtls_ssl_conf_ca_chain(&conf, &ca_cert, nullptr);
                        //mbedtls_ssl_conf_verify(&conf, verify, nullptr);

                        res = mbedtls_ssl_setup(&ssl, &conf);

                        if (res != 0)
                        {
                            Log::error("MBedTLSContext", Format("mbedtls_ssl_setup returned {1}", Int32(res)));
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
                mbedtls_net_free(&connection_identifier);
            }
        }
    }
}