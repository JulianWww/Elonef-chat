#include <elonef-chat/client.hpp>



Chat::Client::Client(const char* request_uri) : Elonef::ClientConnectionHandler(request_uri) {

}

void Chat::Client::setClientKey(Elonef::PublicClientKey& key) {
    CryptoPP::ByteQueue queue = key.toQueue();
    this->make_api_request("set_public_key", queue, false);
}


std::set<std::string> Chat::Client::getAllUsers() {
    CryptoPP::ByteQueue tosend;
    CryptoPP::ByteQueue got = this->make_api_request("get_all_users", tosend, false);
    std::vector<std::string> users = Elonef::toIterable(got, Elonef::toDynamicSizeString_long, Elonef::make_vector<std::string>);
    return std::set(users.begin(), users.end());
}

std::string Chat::Client::addChat(const std::string& chat_name, const std::set<std::string>& users) {
    CryptoPP::ByteQueue tosend = Elonef::toBytes(chat_name);
    Elonef::toBytes(users.begin(), users.end()).TransferAllTo(tosend);
    CryptoPP::ByteQueue got = this->make_api_request("add_chat", tosend, true);
    return Elonef::toBase64(got);
}

Chat::ChatSet Chat::Client::getAllChats() {
    CryptoPP::ByteQueue tosend;
    CryptoPP::ByteQueue got = this->make_api_request("get_chats", tosend, true);
    std::vector<ChatData> chats = Elonef::toIterable(got, ChatData::load, Elonef::make_vector<ChatData>);
    return ChatSet(chats.begin(), chats.end());
}