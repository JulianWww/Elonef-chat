#pragma once

#include <elonef-communication/handlers/serverConnectionHandler.hpp>
#include "database.hpp"

namespace Chat {
    class Server {
        private: Database db;
        private: Elonef::ServerConnectionHandler server;

        public: Server(const std::string& ip, const size_t& port, const sql::SQLString &hostName, const sql::SQLString &userName, const sql::SQLString &password, const sql::SQLString &dbname);

        public: void run();

        private: CryptoPP::ByteQueue setClientKey(CryptoPP::ByteQueue& content, Elonef::ServerConnectionData& connData);
        private: CryptoPP::ByteQueue getAllUsers(CryptoPP::ByteQueue& content, Elonef::ServerConnectionData& connData);
        private: CryptoPP::ByteQueue addChat(CryptoPP::ByteQueue& content, Elonef::ServerConnectionData& connData);
        private: CryptoPP::ByteQueue getChats(CryptoPP::ByteQueue& content, Elonef::ServerConnectionData& connData);

        // internal callbacks
        public: CryptoPP::ByteQueue* getChatKey(const std::pair<std::string, CryptoPP::ByteQueue>& key, const std::string& userid);
        public: std::pair<CryptoPP::ByteQueue*, CryptoPP::ByteQueue*> getNewestChatKey(const std::string& key, const std::string& userid);
        public: void setChatKey(const std::string& userid, const std::string& chat_id, CryptoPP::ByteQueue key_id, const std::vector<std::pair<std::string, CryptoPP::ByteQueue>>& keys);
        public: void addMessage(const std::string& userid, const std::string& chat_id, const CryptoPP::ByteQueue& message);
        public: std::list<std::pair<size_t, CryptoPP::ByteQueue>> getMessages(const std::string& userid, const std::string& chat_id, const size_t& idx, const size_t& size);
    };
}