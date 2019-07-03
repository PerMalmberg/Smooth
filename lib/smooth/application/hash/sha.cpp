#include <smooth/application/hash/sha.h>

namespace smooth::application::hash
{
    std::array<uint8_t, 32> sha256(uint8_t* data, std::size_t len)
    {
        std::array<uint8_t,32> buff{};
        mbedtls_sha256_ret(data, len, buff.data(), 0);
        return buff;
    }
}