#include <elonef-chat/display/render.hpp>
#include <elonef-chat/display/box.hpp>
#include <elonef-chat/display/ralign.hpp>
#include <elonef-chat/io.hpp>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <elonef-communication/utils.hpp>



void Chat::render(const std::vector<Elonef::Message>& messages) {
    for (const Elonef::Message& msg : messages) {
        render(msg);
    }
}

void Chat::render(const Elonef::Message& msg) {
    if (msg.invalid) {render_invalid(); return;}
    switch (msg.type) {
        case 0x00 : {
            render0x00(msg);
            break;
        }
        default : {
            render_invalid();
            break;
        }
    }
    
}

void Chat::render0x00(const Elonef::Message& msg) {
    Box box;
    Ralign ralign;
    ralign.setWidth(box.innerSize());

    box << Elonef::toString( msg.message ) << std::endl;
    ralign << "- \"" << msg.sender_id << "\"" << std::endl;
    ralign << "   "; renderTime(ralign, msg.upload_time);
    box << ralign;

    std::cout << box;
}

void Chat::renderTime(std::ostream& out, const time_t time) {
    time_t rawtime = time;
    struct tm * timeinfo;

    //time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    out << std::string(asctime (timeinfo));
}

void Chat::render_invalid() {
    std::cout << "ERROR\n";
}
