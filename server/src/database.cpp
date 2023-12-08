#include <elonef-chat/database.hpp>
#include <iostream>
#include <filesystem>
#include <elonef-chat/createTables.hpp>
#include <exception>

#define NULL_ON_FAIL(status) if (!status.ok()) {return nullptr;}
#define PREPARE_STATEMENT(statement, str) this->statement.reset(this->con->prepareStatement(str))
#define CONCAT(A, B, C, D) A##B##C##D
#define HANDLE_ERROR(type) catch(type& err) {this->mutex.unlock(); throw err;}
#define MUTEX_TRY(x) 
#define MUTEX_CATCH_ALL 
#define MUTEX_BLOCK(x) \
    this->mutex.lock(); try {x} \
    catch(Elonef::ForwardedError& err){this->mutex.unlock(); throw err;} \
    catch(std::runtime_error& err){this->mutex.unlock();throw err;} \
    catch (...) {this->mutex.unlock(); throw std::runtime_error("error in mutex block");} this->mutex.unlock()

std::string operator*(const std::string& str, const size_t& num) {
    std::string out;
    for (size_t idx=0; idx<num; idx++) {
        out = out + str;
    }
    return out;
}



CryptoPP::ByteQueue* toQueuePtr(const std::string& str) {
    CryptoPP::ByteQueue* ptr = new CryptoPP::ByteQueue(str.size());
    CryptoPP::ByteQueue source = Elonef::fromBase64(str);
    source.TransferAllTo(*ptr);
    return ptr;
}



Chat::Database::Database(const sql::SQLString &hostName, const sql::SQLString &userName, const sql::SQLString &password, const sql::SQLString &dbname) {
    sql::Driver* driver = get_driver_instance();
    this->con = driver->connect(hostName, userName, password);
    this->con->setSchema(dbname);
    createTables(this->con);

    PREPARE_STATEMENT(addChatStatement, "INSERT INTO chats(uid, name) VALUES (?, ?)");
    PREPARE_STATEMENT(getChatsStatement, "SELECT uid, name FROM chats WHERE uid IN(SELECT cid FROM chat_membership WHERE uid = ?);");
    PREPARE_STATEMENT(userIsInChat, "SELECT uid FROM chat_membership WHERE uid=? AND cid=?");
    PREPARE_STATEMENT(getPublicKeyStatement, "SELECT public_key FROM user_keys WHERE uid=?");
    PREPARE_STATEMENT(setPublicKeyStatement, "INSERT INTO user_keys VALUES (?, ?)");
    PREPARE_STATEMENT(getChatKeyStatement, "SELECT chat_key FROM chat_keys WHERE cid=? AND kid=? AND uid=?");
    PREPARE_STATEMENT(getNewestChatKeyStatement, "SELECT newest_key_id FROM chats WHERE uid = ?");
    PREPARE_STATEMENT(setNewestChatKeyStatement, "UPDATE chats SET newest_key_id = ? WHERE uid = ?");
    PREPARE_STATEMENT(addMessageStatement, "INSERT INTO messages(cid, msg) VALUES (?, ?)");
    PREPARE_STATEMENT(getMessagesStatement, "SELECT msgid, msg FROM messages WHERE cid = ? ORDER BY msgid DESC LIMIT ?, ?");
}

Chat::Database::~Database() {
    delete this->con;
    //delete this->driver;
}

std::list<std::string> Chat::Database::getAllUsers() {
    std::list<std::string> users;
    MUTEX_BLOCK(
        std::unique_ptr<sql::Statement> stmt(this->con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT uid FROM user_keys"));
        while (res->next()) {
            users.push_back(res->getString("uid"));
        }
    );
    return users;
}


CryptoPP::ByteQueue Chat::Database::addChat(const std::string& chat_name, const std::unordered_set<std::string>& users) {
   
    CryptoPP::ByteQueue uid = Elonef::uuid();
    CryptoPP::ByteQueue uuid_copy(uid);
    std::string uid_str = Elonef::toBase64(uid);
    this->addUsersToChat(users, uid_str);
    MUTEX_BLOCK(
        this->addChatStatement->setString(1, uid_str);
        this->addChatStatement->setString(2, chat_name);
        this->addChatStatement->execute();
    );
    return uuid_copy;
}

std::list<Chat::ChatData> Chat::Database::getAllChats(const std::string& username) {
    std::list<Chat::ChatData> out;
    MUTEX_BLOCK(
        this->getChatsStatement->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(this->getChatsStatement->executeQuery());
        while (res->next()) {
            out.push_back(ChatData(
                res->getString("name").asStdString(),
                res->getString("uid").asStdString()
            ));
        }
    );
    return out;
}

bool Chat::Database::isInChat(const std::string& chat_name, const std::string& uid) {
    bool contains = false;
    MUTEX_BLOCK(
        this->userIsInChat->setString(1, uid);
        this->userIsInChat->setString(2,chat_name);
        std::unique_ptr<sql::ResultSet> res(this->userIsInChat->executeQuery());
        contains = res->rowsCount() != 0;
    );
    return contains;
}

Elonef::PublicClientKey* Chat::Database::getPublicKey(const std::string& id) {
    std::string str;
    MUTEX_BLOCK(
        this->getPublicKeyStatement->setString(1, id);
        std::unique_ptr<sql::ResultSet> res(this->getPublicKeyStatement->executeQuery());
        if (res->rowsCount()) {
            res->next();
            str = res->getString("public_key");
        }
        else {
            this->mutex.unlock();
            return nullptr;
        }
    );
    CryptoPP::ByteQueue queue = Elonef::fromBase64(str);
    return new Elonef::PublicClientKey(queue);
}

void Chat::Database::setPublicKey(const std::string& id, CryptoPP::ByteQueue& data) {
    MUTEX_BLOCK(
        this->setPublicKeyStatement->setString(1, id);
        this->setPublicKeyStatement->setString(2, Elonef::toBase64(data));
        this->setPublicKeyStatement->execute();
    );
}


std::string buildChatKeyKey(const std::string& chat_id, const CryptoPP::ByteQueue& key_id, const std::string& userid) {
    CryptoPP::ByteQueue queue = Elonef::toBytes(chat_id);
    Elonef::toBytes(key_id).TransferAllTo(queue);
    Elonef::toBytes(userid).TransferAllTo(queue);
    return Elonef::toBase64(queue);
}

CryptoPP::ByteQueue* Chat::Database::getChatKey(const std::string& chat_id, CryptoPP::ByteQueue key_id_queue, const std::string& userid) {
    std::string key_id = Elonef::toBase64(key_id_queue);
    std::string key;
    MUTEX_BLOCK(
        this->getChatKeyStatement->setString(1, chat_id);
        this->getChatKeyStatement->setString(2, key_id);
        this->getChatKeyStatement->setString(3, userid);
        std::unique_ptr<sql::ResultSet> res(this->getChatKeyStatement->executeQuery());
        res->next();
        key = res->getString("chat_key");
    );
    return toQueuePtr(key);
}

void Chat::Database::setChatKey(const std::string& chat_id, const std::string& key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys) {
    if (keys.size() == 0) { return; }
    std::string query = "INSERT INTO chat_keys VALUES " + std::string("(?, ?, ?, ?),") * (keys.size()-1) + "(?, ?, ?, ?)";
    MUTEX_BLOCK(
        std::unique_ptr<sql::PreparedStatement> stmt(this->con->prepareStatement(query));
        size_t idx=1;
        for (const std::pair<std::string, CryptoPP::ByteQueue>& key : keys) {
            stmt->setString(idx, chat_id);
            stmt->setString(idx+1, key_id);
            stmt->setString(idx+2, key.first);
            CryptoPP::ByteQueue key_content_queue(key.second);
            std::string key_content = Elonef::toBase64(key_content_queue);
            stmt->setString(idx+3, key_content);
            idx = idx+4;
        }
        stmt->execute();
    );
}

CryptoPP::ByteQueue* Chat::Database::getNewestChatKey(const std::string& chat_id) {
    std::string key_id;
    MUTEX_BLOCK(
        this->getNewestChatKeyStatement->setString(1, chat_id);
        std::unique_ptr<sql::ResultSet> res(this->getNewestChatKeyStatement->executeQuery());
        res->next();
        key_id = res->getString("newest_key_id");
    );
    return toQueuePtr(key_id);
}

void Chat::Database::setNewestChatKey(const std::string& chat_id, const std::string& key_id) {
    MUTEX_BLOCK(
        this->setNewestChatKeyStatement->setString(1, key_id);
        this->setNewestChatKeyStatement->setString(2, chat_id);
        this->setNewestChatKeyStatement->execute();
    );
}

std::list<std::pair<size_t, CryptoPP::ByteQueue>> Chat::Database::getMessages(const std::string& chat_id, const size_t& idx, const size_t& size) {
    std::list<std::pair<size_t, CryptoPP::ByteQueue>> out;
    MUTEX_BLOCK(
        this->getMessagesStatement->setString(1, chat_id);
        this->getMessagesStatement->setUInt64(2, idx);
        this->getMessagesStatement->setUInt64(3, size);
        std::unique_ptr<sql::ResultSet> res(this->getMessagesStatement->executeQuery());
        while (res->next()) {
            out.push_front(
                {   
                    res->getUInt64("msgid"),
                    Elonef::fromBase64(res->getString("msg"))
                }
            );
        }
    );
    return out;
}

void Chat::Database::addMessages(const std::string& chat_id, CryptoPP::ByteQueue message) {
    std::string msg = Elonef::toBase64(message);
    MUTEX_BLOCK(
        this->addMessageStatement->setString(1, chat_id);
        this->addMessageStatement->setString(2, msg);
        this->addMessageStatement->execute();
    );
}



void Chat::Database::addUsersToChat(const std::unordered_set<std::string>& users, const std::string& chat_id) {
    if (users.size() == 0) {
        return;
    }
    std::string stmt_str = "INSERT INTO chat_membership VALUES " + std::string("(?, ?),")*(users.size()-1) + "(?, ?);";
    MUTEX_BLOCK(
        std::unique_ptr<sql::PreparedStatement> stmt(this->con->prepareStatement(stmt_str));
        size_t idx=1;
        for (const std::string& user : users) {
            stmt->setString(idx, chat_id);
            stmt->setString(idx+1, user);
            idx += 2;
        }
        stmt->execute();
    );
}