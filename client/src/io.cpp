#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include <cstdlib>
#include <elonef-chat/io.hpp>
#include <iostream>
#include <sstream>

void SetStdinEcho(bool enable = true)
{
#ifdef WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if( !enable )
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;

    SetConsoleMode(hStdin, mode );

#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if( !enable )
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

std::string Chat::getPassword() {
    SetStdinEcho(false);

    std::string password = getUserInput();

    SetStdinEcho(true);
    return password;
}

std::string Chat::getUserInput() {
    std::stringstream txt;
    char c;
    while (true) {
        c = std::cin.get();
        if (c=='\n') {
            break;
        }
        txt << c;
    }
    return txt.str();
}

void Chat::handleExit() {
    SetStdinEcho(true);
    std::cout << std::endl;
}