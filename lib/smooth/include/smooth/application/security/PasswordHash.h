#pragma once

#include <tuple>
#include <smooth/core/logging/log.h>
#include "sodium.h"

using namespace smooth::core::logging;

namespace smooth
{
    namespace application
    {
        namespace security
        {
            class PasswordHash
            {
                public:
                    PasswordHash(size_t memory_limit = 8 * 1024)
                        :mem_limit(memory_limit)
                    {
                        if (sodium_init() < 0)
                        {
                            Log::error("PasswordHash", "Cannot initialize libsodium");
                        }
                    }

                    std::tuple<bool, std::string> hash(const std::string& password, size_t computation_count = crypto_pwhash_OPSLIMIT_SENSITIVE) const;
                    bool verify_password_to_hash(const std::string& password, const std::string& hash);

                private:                    
                    size_t mem_limit;
            };
        }
    }
    
}