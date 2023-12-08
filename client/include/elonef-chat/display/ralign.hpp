#pragma once

#include <sstream>
#include <list>

namespace Chat {
    class Ralign : public std::stringstream {
        private: unsigned int width;

        public: std::string str() const;
        public: void setWidth(const unsigned int& width);

        private: static std::pair<unsigned int, std::list<std::string>> getTxtBlockWidth(const std::string& str);
    };
}

std::ostream& operator<<(std::ostream& out, const Chat::Ralign& aligner);