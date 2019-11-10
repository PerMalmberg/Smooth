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

#include "smooth/core/network/MbedTLSContext.h"
#include <mbedtls/error.h>
#include <memory>
#include "smooth/core/logging/log.h"
#include <cstring>

#if defined(ESP_PLATFORM) && defined(CONFIG_MBEDTLS_DEBUG)
    #include <mbedtls/esp_debug.h>
#endif

using namespace smooth::core::logging;

namespace smooth::core::network
{
    // Things to improve:
    // https://tls.mbed.org/discussions/generic/one-time-and-per-session-initialization-functions

    static constexpr const char* tag = "MBedTLSContext";

#ifndef ESP_PLATFORM

    static void mbedtls_debug(void* /*ctx*/, int /*level*/, const char* /*file*/, int /*line*/, const char* str)
    {
        Log::debug(tag, "{}", str);
    }

#endif

    void log_mbedtls_error(const char* log_tag, const char* prefix, int err_code) noexcept
    {
        char buf[128];
        mbedtls_strerror(err_code, buf, sizeof(buf));
        Log::error(log_tag, "{} returned {}: {}", prefix, err_code, buf);
    }

    MBedTLSContext::MBedTLSContext()
    {
        mbedtls_pk_init(&pk_key);
        mbedtls_ssl_config_init(&conf);
        mbedtls_x509_crt_init(&ca_cert);
        mbedtls_x509_crt_init(&ca_chain);
        mbedtls_x509_crt_init(&server_cert);
        mbedtls_entropy_init(&entropy);
        mbedtls_ctr_drbg_init(&ctr_drbg);

#if defined(ESP_PLATFORM)
    #if defined(CONFIG_MBEDTLS_DEBUG)
        mbedtls_esp_enable_debug_log(&conf, 1);
    #endif
#else
        mbedtls_ssl_conf_dbg(&conf, mbedtls_debug, stdout);
        mbedtls_debug_set_threshold(1);
#endif
    }

    int MBedTLSContext::common_init(bool server)
    {
        auto res = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, nullptr, 0);

        if (res != 0)
        {
            log_mbedtls_error(tag, "mbedtls_ctr_drbg_seed returned", res);
        }
        else
        {
            res = mbedtls_ssl_config_defaults(&conf,
                                              server ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT,
                                              MBEDTLS_SSL_TRANSPORT_STREAM,
                                              MBEDTLS_SSL_PRESET_DEFAULT);

            if (res != 0)
            {
                log_mbedtls_error(tag, "mbedtls_ssl_config_defaults", res);
            }
            else
            {
                mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
            }
        }

        return res;
    }

    bool MBedTLSContext::init_client(const std::vector<unsigned char>& ca_certificates)
    {
        auto res = common_init(false);

        if (res == 0)
        {
            if (ca_certificates.empty())
            {
                mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
            }
            else
            {
                mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);

                res = load_certificate(ca_certificates, ca_cert);

                if (res == 0)
                {
                    mbedtls_ssl_conf_ca_chain(&conf, &ca_cert, nullptr);
                }
            }
        }

        return res == 0;
    }

    bool MBedTLSContext::init_server(const std::vector<unsigned char>& ca_certificates,
                                     const std::vector<unsigned char>& server_certificate,
                                     const std::vector<unsigned char>& private_key,
                                     const std::vector<unsigned char>& password)
    {
        auto res = common_init(true);

        if (res == 0)
        {
            res = load_certificate(server_certificate, server_cert);

            if (res == 0)
            {
                res = load_certificate(ca_certificates, ca_cert);
            }

            if (res == 0)
            {
                res = mbedtls_pk_parse_key(&pk_key,
                                           private_key.data(),
                                           private_key.size(),
                                           password.data(),
                                           password.size());

                if (res != 0)
                {
                    log_mbedtls_error(tag, "mbedtls_pk_parse_key", res);
                }
                else
                {
                    mbedtls_ssl_conf_ca_chain(&conf, &ca_cert, nullptr);
                    res = mbedtls_ssl_conf_own_cert(&conf, &server_cert, &pk_key);

                    if (res != 0)
                    {
                        log_mbedtls_error(tag, "mbedtls_ssl_conf_own_cert", res);
                    }
                }
            }
        }

        return res;
    }

    MBedTLSContext::~MBedTLSContext()
    {
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_x509_crt_free(&ca_chain);
        mbedtls_x509_crt_free(&ca_cert);
        mbedtls_x509_crt_free(&server_cert);
        mbedtls_ssl_config_free(&conf);
        mbedtls_pk_free(&pk_key);
    }

    int MBedTLSContext::load_certificate(const std::vector<unsigned char>& cert, mbedtls_x509_crt& target)
    {
        auto res = mbedtls_x509_crt_parse(&target, cert.data(), cert.size());

        if (res < 0)
        {
            log_mbedtls_error(tag, "mbedtls_x509_crt_parse", res);
        }
        else if (res > 0)
        {
            Log::error("MBedTLSContext", "mbedtls_x509_crt_parse failed to parse {} certificates.", res);
        }

        return res;
    }

    std::unique_ptr<SSLContext> MBedTLSContext::create_context()
    {
        auto context = std::make_unique<SSLContext>();
        auto res = mbedtls_ssl_setup(*context, &conf);

        if (res != 0)
        {
            log_mbedtls_error(tag, "mbedtls_ssl_setup", res);
        }

        return context;
    }
}
