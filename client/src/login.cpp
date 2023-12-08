#include <elonef-chat/login.hpp>
#include <elonef-chat/io.hpp>
#include <elonef-communication/encryption/aes.hpp>
#include <iostream>
#include <cryptopp/files.h>
#include <elonef-chat/hash.hpp>
#include <filesystem>

std::string getConfirmedPassword();

Elonef::ClientKeys getUserName(Chat::Client& client) {
    std::cout << "user name: ";
    std::string name = Chat::getUserInput();

    Elonef::ClientKeys keys = Elonef::generateClientKeys("", {}, name);

    try {
        client.setClientKey(keys.public_key);
    }
    catch (Elonef::RemoteError& err) {
        std::cout << "username already exists\n";
        return getUserName(client);
    }
    return keys;
}

std::string retryPassword() {
    std::cout << "Passwords do not match\n";
    return getConfirmedPassword();
}

std::string getConfirmedPassword() {
    std::cout << "password: ";
    std::string pswd1 = Chat::getPassword();
    std::cout << "\nconfirm password: ";
    std::string pswd2 = Chat::getPassword();
    std::cout << std::endl;
    return pswd1 == pswd2 ? pswd1 : retryPassword();
}

Elonef::PrivateClientKey loginNewKey(Chat::Client& client) {
    Elonef::ClientKeys keys =  getUserName(client);
    CryptoPP::SecByteBlock pswdhash = Chat::sha256(getConfirmedPassword());

    CryptoPP::ByteQueue key_queue = keys.private_key.toQueue();
    CryptoPP::FileSink file(KEY_FILE);
    Elonef::toQueue(Chat::sha256(pswdhash)).TransferAllTo(file);
    Elonef::encrypt(key_queue, pswdhash).TransferAllTo(file);   

    return keys.private_key;
}

CryptoPP::ByteQueue readKeyFile() {
    CryptoPP::FileSource file(KEY_FILE, true);
    CryptoPP::ByteQueue content;
    file.TransferAllTo(content);
    return content;
}

CryptoPP::SecByteBlock getDecryptionKey(CryptoPP::SecByteBlock verifier) {
    CryptoPP::SecByteBlock pswdhash;
    CryptoPP::SecByteBlock pswd2hash;
    std::string errormsg;

    do {
        std::cout << errormsg << "password: SKIPPING FOR TESTING";
        std::string pswd = "d"; //Chat::getPassword();
        pswdhash = Chat::sha256(pswd);
        pswd2hash = Chat::sha256(pswdhash);
        errormsg = "\nPassword incorrect try again!\n";
    } while (pswd2hash != verifier);
    std::cout << std::endl;
    return pswdhash;
}

Elonef::PrivateClientKey loginExistingKey() {
    CryptoPP::ByteQueue file_content = readKeyFile();
    CryptoPP::ByteQueue verifierQueue = Elonef::extractConstantLengthQueue(file_content, CryptoPP::SHA256::DIGESTSIZE);
    CryptoPP::SecByteBlock dec_key = getDecryptionKey(Elonef::toSecBlock(verifierQueue));
    CryptoPP::ByteQueue decrypted = Elonef::decrypt(file_content, dec_key);
    return Elonef::PrivateClientKey(decrypted);
}
  
Elonef::PrivateClientKey Chat::login(Client& client) {
    if (std::filesystem::exists(KEY_FILE)) {
        return loginExistingKey();
    }
    return loginNewKey(client);
}