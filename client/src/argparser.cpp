#include <elonef-chat/arg_parser.hpp>
#include <elonef-chat/subcommands.hpp>
#include <CLI/CLI.hpp>
#include <iostream>

int Chat::parseArgs(int argc, char **argv) {
    CLI::App app("Elonef Chat CLI");
    app.set_help_all_flag("--help-all", "Expand all help");


    // make chat
    CLI::App* mkchat = app.add_subcommand("mkchat", "create a new chat");
    std::string chat_name;
    mkchat->add_option("-n,--name", chat_name, "the name of the chat")->required();
    std::vector<std::string> chat_members;
    mkchat->add_option("-m,--members", chat_members, "the users that are part of the chat")->required()->expected(1, (unsigned int)-1);
    app.require_subcommand(1);

    // login
    CLI::App* login = app.add_subcommand("login", "runs just to login (Debug only)");

    // send message
    CLI::App* sendMessage = app.add_subcommand("send", "send a message to a specific chat");
    sendMessage->add_option("-c,--chat", chat_name, "the name of the chat to send the message into")->required();
    std::string message;
    sendMessage->add_option("-m,--message", message, "the message top send")->required();

    // get messages
    CLI::App* getMessages = app.add_subcommand("get", "get a ceratain subset of messages from the server and display them to the screen");
    getMessages->add_option("-c,--chat", chat_name, "the name of the chat to read messages from")->required();
    size_t msg_idx;
    getMessages->add_option("-i,--idx", msg_idx, "the back indes of the last message to read")->default_val(0);
    size_t msg_count;
    getMessages->add_option("-n,--number", msg_count, "the amount of messages to read")->default_val(10);

    // get chats
    CLI::App* pull = app.add_subcommand("pull", "pulls the list of remote chats to local chat storate");

    CLI11_PARSE(app, argc, argv);

    std::string subcommand_name = app.get_subcommands().front()->get_name();

    if (subcommand_name == "mkchat") {
        return Chat::runMakeChat(chat_name, chat_members);
    }
    else if (subcommand_name == "login") {
        return Chat::runLogin();
    }
    else if (subcommand_name == "send") {
        return Chat::runSend(chat_name, message);
    }
    else if (subcommand_name == "get") {
        return Chat::runGet(chat_name, msg_idx, msg_count);
    }
    else if (subcommand_name == "pull") {
        return Chat::runPull();
    }

    return 0;
}