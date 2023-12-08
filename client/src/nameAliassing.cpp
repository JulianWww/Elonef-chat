#include <elonef-chat/nameAliassing.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

Chat::NameAliassing::NameAliassing(const std::string& name) : filename(name) {
    std::ifstream in(name);
    if (!in.is_open()) {
        return;
    }
    nlohmann::json json = nlohmann::json::parse(in);
    this->map = json.get<std::unordered_map<std::string, std::string>>();
}

Chat::NameAliassing::~NameAliassing() {
    this->save();
}

void Chat::NameAliassing::save() {
    nlohmann::json json(this->map);
    std::ofstream out(this->filename);
    out << json.dump(2);
    out.close();
}

bool Chat::NameAliassing::hasId(const std::string& id) {
    for (const std::pair<std::string, std::string>& entry : this->map) {
        if (entry.second == id) {
            return true;
        }
    }
    return false;
}