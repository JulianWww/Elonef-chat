#pragma once

#include <memory>
#include <unordered_map>
#include <elonef-communication/keys/genKeys.hpp>
#include <elonef-communication/handlers/serverConnectionHandler.hpp>
#include <mysql_connection.h>
#include <prepared_statement.h>
#include <driver.h>
#include <exception.h>
#include <resultset.h>
#include <statement.h>
#include <mutex>
#include <elonef-chat/chat.hpp>

namespace Chat {
    class Database {
        private: sql::Connection *con;
        private: std::unique_ptr<sql::PreparedStatement> addChatStatement;
        private: std::unique_ptr<sql::PreparedStatement> getChatsStatement;
        private: std::unique_ptr<sql::PreparedStatement> userIsInChat;
        private: std::unique_ptr<sql::PreparedStatement> getPublicKeyStatement;
        private: std::unique_ptr<sql::PreparedStatement> setPublicKeyStatement;
        private: std::unique_ptr<sql::PreparedStatement> getChatKeyStatement;
        private: std::unique_ptr<sql::PreparedStatement> getNewestChatKeyStatement;
        private: std::unique_ptr<sql::PreparedStatement> setNewestChatKeyStatement;
        private: std::unique_ptr<sql::PreparedStatement> addMessageStatement;
        private: std::unique_ptr<sql::PreparedStatement> getMessagesStatement;
        private: std::mutex mutex;

        public: Database(const sql::SQLString &hostName, const sql::SQLString &userName, const sql::SQLString &password, const sql::SQLString &dbname);
        public: ~Database();

        public: std::list<std::string> getAllUsers();
        public: CryptoPP::ByteQueue addChat(const std::string& chat_name, const std::unordered_set<std::string>& users);
        public: std::list<ChatData> getAllChats(const std::string& username);
        public: bool isInChat(const std::string& chat_name, const std::string& uid);

        public: Elonef::PublicClientKey* getPublicKey(const std::string& id);
        public: void setPublicKey(const std::string& id, CryptoPP::ByteQueue& data);

        public: CryptoPP::ByteQueue* getChatKey(const std::string& chat_id, CryptoPP::ByteQueue key_id, const std::string& userid);
        public: void setChatKey(const std::string& chat_id, const std::string& key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys);

        public: CryptoPP::ByteQueue* getNewestChatKey(const std::string& chat_id);
        public: void setNewestChatKey(const std::string& chat_id, const std::string& key_id);

        public: std::list<std::pair<size_t, CryptoPP::ByteQueue>> getMessages(const std::string& chat_id, const size_t& idx, const size_t& size);
        public: void addMessages(const std::string& chat_id, CryptoPP::ByteQueue message);



        private: void addUsersToChat(const std::unordered_set<std::string>& users, const std::string& chat_id);
    };
}