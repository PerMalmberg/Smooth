// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

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