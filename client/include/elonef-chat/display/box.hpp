#pragma once

#include <sstream>
#include <ostream>

namespace Chat {
    class Box: public std::stringstream {
        public: std::string str() const;

        public: unsigned int innerSize() const;
    };
}

std::ostream& operator<<(std::ostream& stream, const Chat::Box& box);