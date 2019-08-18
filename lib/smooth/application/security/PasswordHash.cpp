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

#include <smooth/application/security/PasswordHash.h>
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
                                                          static_cast<decltype(crypto_pwhash_OPSLIMIT_MIN)>(computation_count)),
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
