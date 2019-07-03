#pragma once

#include <array>
#include <mbedtls/sha256.h>

namespace smooth::application::hash
{
    std::array<uint8_t, 32> sha256(uint8_t* data, std::size_t len);
}