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

#include <tuple>
#include <smooth/core/logging/log.h>
#include "sodium.h"

using namespace smooth::core::logging;

namespace smooth::application::security
{
    class PasswordHash
    {
        public:
            PasswordHash(size_t memory_limit = 8 * 1024)
                    : mem_limit(memory_limit)
            {
                if (sodium_init() < 0)
                {
                    Log::error("PasswordHash", "Cannot initialize libsodium");
                }
            }

            std::tuple<bool, std::string>
            hash(const std::string& password, size_t computation_count = crypto_pwhash_OPSLIMIT_SENSITIVE) const;

            bool verify_password_against_hash(const std::string& password, const std::string& hash);

        private:
            size_t mem_limit;
    };
}
