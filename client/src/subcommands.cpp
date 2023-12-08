#include <elonef-chat/subcommands.hpp>
#include <elonef-chat/client.hpp>
#include <elonef-chat/login.hpp>
#include <elonef-chat/nameAliassing.hpp>
#include <elonef-chat/display/render.hpp>
#include <elonef-chat/io.hpp>
#include <algorithm>
#include <iostream>

#define REQUEST_URI "ws://127.0.0.1:9008"
#define ERROR_MISSING_CHAT if (!nameAliassing.map.contains(chat_name)) { \
        std::cout << "chat name: \"" << chat_name << "\" is not known locally:\n\n\tTry running \"" << EXE_NAME << " pull\" to update local chats\n"; \
        return 1; \
    }
#define CLIENT_AND_AUTH Client client(REQUEST_URI); \
    client.connect(); \
    Elonef::PrivateClientKey key = Chat::login(client); \
    client.authenticate(key)


Chat::NameAliassing nameAliassing("chats.json");

int Chat::runMakeChat(const std::string& chat_name, const std::vector<std::string>& _members) {
    if (nameAliassing.map.contains(chat_name)) {
        std::cout << "chat \"" << chat_name << "\" already exists locally\n";
        return 1;
    }
    Client client(REQUEST_URI);
    client.connect();
    std::set<std::string> chat_members(_members.begin(), _members.end());
    std::set<std::string> users = client.getAllUsers();
    std::vector<std::string> invalid_members;
    std::set_difference(chat_members.begin(), chat_members.end(), users.begin(), users.end(), std::back_inserter(invalid_members));

    if (invalid_members.size()) {
        std::cout << "The following users do not exist: " << invalid_members << std::endl;
        return 1;
    }

    Elonef::PrivateClientKey key = Chat::login(client);
    client.authenticate(key);
    chat_members.insert(key.uid);
    if (chat_members.size() < 2) {
        std::cout << "A Chat must contain at least 2 users" << std::endl;
        return 1;
    }
    std::string chat_id = client.addChat(chat_name, chat_members);
    std::unordered_set<std::string> u_chat_members(chat_members.begin(), chat_members.end());
    client.generate_chat_key(u_chat_members, chat_id);

    client.stop();
    nameAliassing.map.insert({chat_name, chat_id});
    return 0;
}

int Chat::runLogin() {
    Client client(REQUEST_URI);
    client.connect();
    Elonef::PrivateClientKey key = Chat::login(client);
    return 0;
}



int Chat::runSend(const std::string& chat_name, const std::string& message) {
    ERROR_MISSING_CHAT;
    CLIENT_AND_AUTH;

    
    CryptoPP::ByteQueue msg = Elonef::toQueue(message);
    client.send_message(msg, 0x00, nameAliassing.map.at(chat_name));

    client.stop();
    return 0;
}

int Chat::runGet(const std::string& chat_name, const size_t& msg_idx, const size_t& msg_count) {
    ERROR_MISSING_CHAT;
    CLIENT_AND_AUTH;
    std::vector<Elonef::Message> messages = client.read_messages(nameAliassing.map.at(chat_name), msg_idx, msg_count);
    render(messages);
    return 6;
}

int Chat::runPull() {
    CLIENT_AND_AUTH;
    ChatSet chats = client.getAllChats();
    bool hasAtLeastOne = false;
    for (const ChatData& chat : chats) {
        if (nameAliassing.hasId(chat.id)) {
            continue;
        }
        hasAtLeastOne = true;
        std::cout << "the chat discribed as: \n\n\t" << chat.name << "\n\nis unknown locally. What would you like to name it? [emptry to skip]: " << std::flush;
    handleInput:
        std::string name = getUserInput();
        if (name.size() == 0) {
            continue;
        }
        if (!nameAliassing.map.contains(name)) {
            nameAliassing.map.insert({name, chat.id});
            continue;
        }
        std::cout << "name already exists. Try again [emptry to skip]: " << std::flush;
        goto handleInput;        
    }
    if (!hasAtLeastOne) {
        std::cout << "no new chats found" << std::endl;
        return 0;
    }
    std::cout << "all chats have been assigned or skiped." << std::endl;
    return 0;
}