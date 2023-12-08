#include <elonef-chat/arg_parser.hpp>
#include <cstdlib>
#include <elonef-chat/io.hpp>
#include <csignal>

void signal_callback_handler(int signum)
{
   exit(signum);
}

int main(int argc, char** argv)
{
    std::atexit(Chat::handleExit);
    std::signal(SIGINT, signal_callback_handler);
    return Chat::parseArgs(argc, argv);
}