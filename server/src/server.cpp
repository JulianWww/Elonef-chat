#include <elonef-chat/server.hpp>
#include <elonef-communication/utils.hpp>

std::unordered_map<std::string, std::string> ca_keys;
Elonef::ClientKeys* keys;
CryptoPP::ByteQueue chat_key;
CryptoPP::ByteQueue chat_key_id;
std::list<CryptoPP::ByteQueue> messages;

#define FOBID_IF_NOT_IN_CHAT(uid, cid) if (!this->db.isInChat(cid, uid)) {throw Elonef::ForwardedError("operation not permitted!");}

CryptoPP::ByteQueue* get_chat_key(const std::pair<std::string, CryptoPP::ByteQueue>& key, const std::string& userid) {
    return &chat_key;
}

std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*> get_newest_chat_key(const std::string& key, const std::string& userid) {
    return {&chat_key, &chat_key_id};
}

void set_chat_key(const std::string& chat_id, const CryptoPP::ByteQueue& key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys) {
    //std::cout << keys << std::endl;
    //std::cout << chat_key << std::endl;
}

void add_message(const std::string& chat_id, const CryptoPP::ByteQueue& message) {
    messages.push_back(message);
}

std::list<CryptoPP::ByteQueue> get_messages(const std::string& userid, const std::string& chat_id, const size_t& idx, const size_t& size) {
    return messages;
}

CryptoPP::ByteQueue test_api(CryptoPP::ByteQueue& queue, Elonef::ServerConnectionData& connData) {
    //throw Elonef::ForwardedError("ups something went wrong");
    return Elonef::toQueue("test success\n\n");
}

#define API_BIND_POINT(name, func) {this->server.add_api_callback(name, [this](CryptoPP::ByteQueue &content, Elonef::ServerConnectionData &connData){return this->func(content, connData);});}
#define API_BIND_POINT_SEC(name, func) {this->server.add_api_callback(name, [this](CryptoPP::ByteQueue &content, Elonef::ServerConnectionData &connData){return this->func(content, connData);}, true);}


Chat::Server::Server(const std::string& ip, const size_t& port, const sql::SQLString &hostName, const sql::SQLString &userName, const sql::SQLString &password, const sql::SQLString &dbname) : db(hostName, userName, password, dbname), 
        server(ip, port, Elonef::SlidingTimeWindow(10, 30),
            [this](const std::string& id, const std::string& userid){return this->db.getPublicKey(id);},
            [this](const std::pair<std::string, CryptoPP::ByteQueue>& key, const std::string& userid) { return this->getChatKey(key, userid);},
            [this](const std::string& key, const std::string& userid) {return this->getNewestChatKey(key, userid);},
            [this](const std::string& userid, const std::string& chat_id, const CryptoPP::ByteQueue& key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys) { return this->setChatKey(userid, chat_id, key_id, keys);},
            [this](const std::string& userid, const std::string& chat_id, const CryptoPP::ByteQueue& message) { return this->addMessage(userid, chat_id, message); }, 
            [this](const std::string& userid, const std::string& chat_id, const size_t& idx, const size_t& size) { return this->getMessages(userid, chat_id, idx, size); } ) {
    
    API_BIND_POINT("set_public_key",    setClientKey);
    API_BIND_POINT("get_all_users",     getAllUsers);

    API_BIND_POINT_SEC("get_chats", getChats);
    API_BIND_POINT_SEC("add_chat", addChat);
};

void Chat::Server::run() {
    this->server.start();
    this->server.wait();
}

CryptoPP::ByteQueue Chat::Server::setClientKey(CryptoPP::ByteQueue &content, Elonef::ServerConnectionData &connData) {
    CryptoPP::ByteQueue data(content);
    std::string id = Elonef::toDynamicSizeString(content);
    this->db.setPublicKey(id, data);

    return CryptoPP::ByteQueue();
}

CryptoPP::ByteQueue Chat::Server::getAllUsers(CryptoPP::ByteQueue &content, Elonef::ServerConnectionData &connData) {
    std::list<std::string> users = this->db.getAllUsers();
    return Elonef::toBytes(users.begin(), users.end());
}

CryptoPP::ByteQueue Chat::Server::addChat(CryptoPP::ByteQueue &content, Elonef::ServerConnectionData &connData) {
    std::string chat_name = Elonef::toDynamicSizeString(content);

    std::vector<std::string> member_list = Elonef::toIterable(content, Elonef::toDynamicSizeString_long, Elonef::make_vector<std::string>);
    std::unordered_set<std::string> members (member_list.begin(), member_list.end());
    members.insert(connData.uid);
    return this->db.addChat(chat_name, members);
}

CryptoPP::ByteQueue Chat::Server::getChats(CryptoPP::ByteQueue& content, Elonef::ServerConnectionData& connData) {
    std::list<ChatData> chats = this->db.getAllChats(connData.uid);
    return Elonef::toBytes(chats.begin(), chats.end());
}



CryptoPP::ByteQueue* Chat::Server::getChatKey(const std::pair<std::string, CryptoPP::ByteQueue>& key, const std::string& userid) {
    return this->db.getChatKey(key.first, key.second, userid);
}

std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*> Chat::Server::getNewestChatKey(const std::string& chat_id, const std::string& userid) {
    CryptoPP::ByteQueue* key_id = this->db.getNewestChatKey(chat_id);
    CryptoPP::ByteQueue* key = this->db.getChatKey(chat_id, *key_id, userid);
    return {key, key_id};
}

void Chat::Server::setChatKey(const std::string& userid, const std::string& chat_id, CryptoPP::ByteQueue key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys) {
    FOBID_IF_NOT_IN_CHAT(userid, chat_id);
    std::string key_id_str = Elonef::toBase64(key_id);
    this->db.setChatKey(chat_id, key_id_str, keys);
    this->db.setNewestChatKey(chat_id, key_id_str);
}

void Chat::Server::addMessage(const std::string& userid, const std::string& chat_id, const CryptoPP::ByteQueue& message) {
    FOBID_IF_NOT_IN_CHAT(userid, chat_id);
    this->db.addMessages(chat_id, message);
}

std::list<std::pair<size_t, CryptoPP::ByteQueue>> Chat::Server::getMessages(const std::string& userid, const std::string& chat_id, const size_t& idx, const size_t& size) {
    FOBID_IF_NOT_IN_CHAT(userid, chat_id);
    return this->db.getMessages(chat_id, idx, size);
}