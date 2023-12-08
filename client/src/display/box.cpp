#include <elonef-chat/display/box.hpp>
#include <elonef-chat/display/size.hpp>
#include <iostream>

std::stringstream& hline(std::stringstream& stream, const int& width) {
    for (int idx=-4; idx<width; idx++) {
        stream << "#";
    }
    return stream;
}

void line_start(std::stringstream& stream) {
    stream << "# ";
}
void line_end(std::stringstream& stream, int has, const int& should) {
    for (;has<should;has++) {
        stream << " ";
    }
    stream << " #";
}
void line_switch(std::stringstream& stream, int& has, const int& should) {
    line_end(stream, has, should);
    stream << std::endl;
    line_start(stream);
    has = 0;
}

std::string Chat::Box::str() const {
    std::string str = std::stringstream::str();
    std::stringstream out;
    const unsigned int width = this->innerSize();

    hline(out, width) << std::endl;
    line_start(out);

    int wrote_chars = 0;
    for (char c : str) {
        if (wrote_chars >= width) {
            line_switch(out, wrote_chars, width);
        }
        if (c == '\n') {
            line_switch(out, wrote_chars, width);
            continue;
        }
        out << c;
        wrote_chars++;
    }
    line_end(out, wrote_chars, width);
    hline(out, width);

    return out.str();
}

unsigned int Chat::Box::innerSize() const {
    return get_terminal_width() - 4;
}

std::ostream& operator<<(std::ostream& stream, const Chat::Box& box) {
    return (stream << box.str());
}