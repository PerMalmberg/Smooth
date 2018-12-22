#include <smooth/application/security/PasswordHash.h>
#include <algorithm>

namespace smooth
{
    namespace application
    {
        namespace security
        {
            std::tuple<bool, std::string> PasswordHash::hash(const std::string& password, size_t computation_count) const
            {          
                
                char out[crypto_pwhash_STRBYTES];
                
                auto result = crypto_pwhash_str(out,
                                      password.c_str(),
                                      password.size(),
                                      std::min(std::max(crypto_pwhash_OPSLIMIT_MIN, computation_count), crypto_pwhash_OPSLIMIT_MAX),
                                      mem_limit) == 0;

                std::string s;
                if(result)
                {
                    s.assign(std::begin(out), std::end(out));
                }
                return std::make_tuple(result, std::move(s));
            }

            bool PasswordHash::verify_password_to_hash(const std::string& password, const std::string& hash)
            {
                char hashed_pwd[crypto_pwhash_STRBYTES];
                std::copy_n(hash.begin(), crypto_pwhash_STRBYTES, std::begin(hashed_pwd));

                return crypto_pwhash_str_verify(hashed_pwd, password.c_str(), password.size()) == 0;
            }
        }
    }
}