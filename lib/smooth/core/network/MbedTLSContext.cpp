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

            static constexpr const char* tag = "MBedTLSContext";
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

            bool MBedTLSContext::init()
            {
                mbedtls_ssl_init(&ssl);
                mbedtls_ssl_config_init(&conf);
                mbedtls_x509_crt_init(&ca_cert);
                mbedtls_entropy_init(&entropy);
                mbedtls_ctr_drbg_init(&ctr_drbg);
#ifndef ESP_PLATFORM
                mbedtls_ssl_conf_dbg(&conf, mbedtls_debug, stdout);
                mbedtls_debug_set_threshold(0);
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
                        mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);

                        mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
                        mbedtls_ssl_conf_ca_chain(&conf, &ca_cert, nullptr);

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
            }
        }
    }
}