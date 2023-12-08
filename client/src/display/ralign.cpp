#include <elonef-chat/display/ralign.hpp>

void pad(std::ostream& out, const unsigned int& padding_size) {
    for (unsigned int idx=0; idx<padding_size; idx++) {
        out << " ";
    }
}

std::string Chat::Ralign::str() const {
    std::string str = std::stringstream::str();
    std::pair<unsigned int, std::list<std::string>> blocks = getTxtBlockWidth(str);
    std::stringstream out;
    bool addEndl = false;
    for (const std::string& block: blocks.second) {
        if (addEndl) {
            out << std::endl;
        }
        else {
            addEndl = true;
        }
        pad(out, this->width - blocks.first);
        out << block;
    }
    return out.str();
}

void Chat::Ralign::setWidth(const unsigned int& width) {
    this->width = width;
}

std::pair<unsigned int, std::list<std::string>> Chat::Ralign::getTxtBlockWidth(const std::string& str) {
    unsigned int width = 0;
    unsigned int max_width = 0;
    std::stringstream line;
    std::list<std::string> lines;
    for (const char& c : str) {
        if (c == '\n') {
            max_width = std::max(max_width, width);
            lines.push_back(line.str());
            line = std::stringstream();
            width = 0;
        }
        else {
            width++;
            line << c;
        }
    }
    if (width) {
        lines.push_back(line.str());
    }
    return {std::max(max_width, width), lines};
}

std::ostream& operator<<(std::ostream& out, const Chat::Ralign& aligner) {
    return (out << aligner.str());
}