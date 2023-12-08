#include <elonef-chat/display/size.hpp>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

int Chat::get_terminal_width() {
#ifdef TIOCGSIZE
    struct ttysize ts;
    ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
    return ts.ts_cols;
#elif defined(TIOCGWINSZ)
    struct winsize ts;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
    return ts.ws_col;
#endif /* TIOCGSIZE */
}