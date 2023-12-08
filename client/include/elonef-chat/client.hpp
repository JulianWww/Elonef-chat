#pragma once

#include <elonef-communication/handlers/clientConnectionHandler.hpp>
#include <set>
#include <elonef-chat/chat.hpp>


namespace Chat {
    typedef std::set<ChatData, ChatData::Compare> ChatSet;

    class Client : public Elonef::ClientConnectionHandler {
        public: Client(const char* request_uri);

        public: void setClientKey(Elonef::PublicClientKey& key);
        public: std::set<std::string> getAllUsers();
        public: std::string addChat(const std::string& chat_name, const std::set<std::string>& users);
        public: ChatSet getAllChats();
    };
}