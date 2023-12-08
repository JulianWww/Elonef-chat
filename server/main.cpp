#include <iostream>
#include <elonef-chat/server.hpp>

int main() {
    Chat::Server server("0.0.0.0", 9008, "tcp://127.0.0.1:3306","test_user","password", "ElonefChat");
    server.run();
    return 0;
}