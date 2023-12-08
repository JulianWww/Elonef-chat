#include <elonef-chat/createTables.hpp>
#include <statement.h>

void createTable(sql::Connection* conn, std::string str) {
    sql::Statement* stmt = conn->createStatement();
    stmt->execute("CREATE TABLE IF NOT EXISTS " + str);
    delete stmt;
}

void createPulicKeyTable(sql::Connection* conn) {
    createTable(conn, "user_keys(uid VARCHAR(256) NOT NULL,public_key LONGTEXT NOT NULL, PRIMARY KEY (uid));");
}

void createChatDataTable(sql::Connection* conn) {
    createTable(conn, "chats(uid VARCHAR(50) NOT NULL,name LONGTEXT NOT NULL, newest_key_id VARCHAR(50), PRIMARY KEY (uid));");
}

void createChatMembershipTable(sql::Connection* conn) {
    createTable(conn, "chat_membership(cid VARCHAR(50) NOT NULL,uid VARCHAR(256) NOT NULL, KEY (cid), KEY(uid));");
}

void createChatKeysTable(sql::Connection* conn) {
    createTable(conn, "chat_keys(cid VARCHAR(50) NOT NULL,kid VARCHAR(50) NOT NULL,uid VARCHAR(256) NOT NULL,chat_key LONGTEXT NOT NULL, KEY (cid), KEY (kid), KEY(uid));");
}

void createChatMessagesTable(sql::Connection* conn) {
    createTable(conn, "messages(cid VARCHAR(50) NOT NULL,msgid int NOT NULL AUTO_INCREMENT, msg LONGTEXT NOT NULL, KEY (cid), PRIMARY KEY (msgid));");
}

void Chat::createTables(sql::Connection* conn) {
    createPulicKeyTable(conn);
    createChatDataTable(conn);
    createChatMembershipTable(conn);
    createChatKeysTable(conn);
    createChatMessagesTable(conn);
}