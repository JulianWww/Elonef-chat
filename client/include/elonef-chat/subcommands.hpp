#pragma once

#include <vector>
#include <string>

namespace Chat {
    int runMakeChat(const std::string& chat_name, const std::vector<std::string>& members);
    int runLogin();
    int runSend(const std::string& chat_name, const std::string& message);
    int runGet(const std::string& chat_name, const size_t& msg_idx, const size_t& msg_count);
    int runPull();
}