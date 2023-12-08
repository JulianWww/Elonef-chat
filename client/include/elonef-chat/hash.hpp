#pragma once

#include <cryptopp/sha.h>
#include <cryptopp/queue.h>

namespace Chat {
    CryptoPP::SecByteBlock sha256(std::string str);
    CryptoPP::SecByteBlock sha256(CryptoPP::SecByteBlock& str);
}