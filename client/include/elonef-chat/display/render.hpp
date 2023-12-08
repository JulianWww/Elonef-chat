#pragma once

#include <elonef-communication/types.hpp>
#include <elonef-chat/chat.hpp>
#include <vector>
#include <ostream>

namespace Chat {
    void render(const std::vector<Elonef::Message>& messages);
    void render(const Elonef::Message& message);
    void render0x00(const Elonef::Message& message);
    void renderTime(std::ostream& out, const time_t time);
    void render_invalid();
}