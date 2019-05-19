#pragma once

#include <vector>
#include <memory>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>

namespace smooth::core::network
{
    void log_mbedtls_error(const char*  log_tag, const char* prefix, int err_code) noexcept;

    class SSLContext
    {
        public:
            SSLContext()
            {
                mbedtls_ssl_init(&ssl);
            }

            ~SSLContext()
            {
                mbedtls_ssl_free(&ssl);
            }

            operator mbedtls_ssl_context*()
            {
                return &ssl;
            }

            mbedtls_ssl_states get_state() const
            {
                return static_cast<mbedtls_ssl_states>(ssl.state);
            }

        private:
            mbedtls_ssl_context ssl{};
    };

    class MBedTLSContext
    {
        public:
            MBedTLSContext();

            ~MBedTLSContext();

            bool init_client(const std::vector<unsigned char>& ca_certificates);

            bool init_server(const std::vector<unsigned char>& ca_certificates,
                             const std::vector<unsigned char>& server_certificate,
                             const std::vector<unsigned char>& private_key,
                             const std::vector<unsigned char>& password);

            std::unique_ptr<SSLContext> create_context();

        private:
            int common_init(bool server);

            int load_certificate(const std::vector<unsigned char>& cert, mbedtls_x509_crt& target);

            mbedtls_entropy_context entropy{};
            mbedtls_ctr_drbg_context ctr_drbg{};
            mbedtls_ssl_config conf{};
            mbedtls_x509_crt ca_cert{};
            mbedtls_x509_crt ca_chain{};
            mbedtls_x509_crt server_cert{};
            mbedtls_pk_context pk_key{};
    };
}