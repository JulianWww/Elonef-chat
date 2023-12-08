#include <elonef-chat/hash.hpp>

CryptoPP::SecByteBlock Chat::sha256(std::string str) {
    CryptoPP::SHA256 hash;
    CryptoPP::SecByteBlock out(CryptoPP::SHA256::DIGESTSIZE);
    hash.Update((CryptoPP::byte*)str.c_str(), str.size());
    hash.Final(out.data());
    return out;
}

CryptoPP::SecByteBlock Chat::sha256(CryptoPP::SecByteBlock& str) {
    CryptoPP::SHA256 hash;
    CryptoPP::SecByteBlock out(CryptoPP::SHA256::DIGESTSIZE);
    hash.Update((CryptoPP::byte*)str.data(), str.size());
    hash.Final(out.data());
    return out;
}