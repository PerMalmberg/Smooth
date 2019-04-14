#pragma once

#include <vector>
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

                    bool init_client(const std::vector<unsigned char>& ca_certificates);

                    mbedtls_ssl_context* get_context() { return &ssl; }
                    const mbedtls_ssl_context* get_context() const { return &ssl; }

                private:

                    mbedtls_entropy_context entropy{};
                    mbedtls_ctr_drbg_context ctr_drbg{};
                    mbedtls_ssl_context ssl{};
                    mbedtls_ssl_config conf{};
                    mbedtls_x509_crt ca_cert{};
            };
        }
    }
}