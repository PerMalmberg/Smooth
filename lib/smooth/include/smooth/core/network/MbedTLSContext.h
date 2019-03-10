#pragma once

#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            class MBedTLSContext
            {
                public:
                    MBedTLSContext() = default;
                    ~MBedTLSContext();

                    bool init();

                    mbedtls_ssl_context* get_context() { return &ssl; }

                private:

                    mbedtls_net_context connection_identifier{};
                    mbedtls_entropy_context entropy{};
                    mbedtls_ctr_drbg_context ctr_drbg{};
                    mbedtls_ssl_context ssl{};
                    mbedtls_ssl_config conf{};
                    mbedtls_x509_crt ca_cert{};
            };
        }
    }
}