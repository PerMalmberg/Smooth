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

#include "smooth/application/security/PasswordHash.h"
#include <algorithm>

namespace smooth::application::security
{
    std::tuple<bool, std::string> PasswordHash::hash(const std::string& password, size_t computation_count) const
    {
        char out[crypto_pwhash_STRBYTES];

        auto result = crypto_pwhash_str(out,
                                        password.c_str(),
                                        password.size(),
                                        std::min(std::max(crypto_pwhash_OPSLIMIT_MIN,
                                                          static_cast<decltype(crypto_pwhash_OPSLIMIT_MIN)>(
                                                              computation_count)),
                                                 crypto_pwhash_OPSLIMIT_MAX),
                                        mem_limit) == 0;

        std::string s;

        if (result)
        {
            s.assign(std::begin(out), std::end(out));
        }

        return std::make_tuple(result, std::move(s));
    }

    bool PasswordHash::verify_password_against_hash(const std::string& password, const std::string& hash)
    {
        char hashed_pwd[crypto_pwhash_STRBYTES];
        std::copy_n(hash.begin(), crypto_pwhash_STRBYTES, std::begin(hashed_pwd));

        return crypto_pwhash_str_verify(hashed_pwd, password.c_str(), password.size()) == 0;
    }
}
