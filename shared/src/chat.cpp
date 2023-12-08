#include <elonef-chat/chat.hpp>
#include <elonef-communication/encoding/decoding.hpp>
#include <elonef-communication/encoding/encoding.hpp>

Chat::ChatData::ChatData(const std::string& _name, const std::string& _id) : name(_name), id(_id) {}

Chat::ChatData::ChatData() {}

Chat::ChatData Chat::ChatData::load(CryptoPP::ByteQueue& queue) {
    return ChatData(Elonef::toDynamicSizeString(queue), Elonef::toDynamicSizeString(queue));
}

CryptoPP::ByteQueue Chat::ChatData::toQueue() const {
    CryptoPP::ByteQueue queue = Elonef::toBytes(this->id);
    Elonef::toBytes(this->name).TransferAllTo(queue);
    return queue;
}

