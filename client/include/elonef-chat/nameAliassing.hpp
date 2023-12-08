#pragma once

#include <unordered_map>
#include <string>

namespace Chat {
    class NameAliassing {
        private: const std::string filename;
        public: std::unordered_map<std::string, std::string> map;

        public: NameAliassing(const std::string& fileName);
        public: ~NameAliassing();

        public: void save();

        public: bool hasId(const std::string& id);
    };
}