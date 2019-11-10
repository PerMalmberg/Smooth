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

#pragma once

#include <tuple>
#include "smooth/core/logging/log.h"
#include "sodium.h"

using namespace smooth::core::logging;

namespace smooth::application::security
{
    class PasswordHash
    {
        public:
            explicit PasswordHash(size_t memory_limit = 8* 1024)
                    : mem_limit(memory_limit)
            {
                if (sodium_init() < 0)
                {
                    Log::error("PasswordHash", "Cannot initialize libsodium");
                }
            }

            [[nodiscard]] std::tuple<bool, std::string> hash(
                const std::string& password, size_t computation_count = crypto_pwhash_OPSLIMIT_SENSITIVE) const;

            bool verify_password_against_hash(const std::string& password, const std::string& hash);

        private:
            size_t mem_limit;
    };
}
