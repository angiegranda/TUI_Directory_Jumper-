#include "UI.h"
#include <cstdlib>
#ifdef _WIN32
    #include <conio.h>
    #define NOMINMAX
    #include <windows.h>
#else
    #include <csignal>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <unistd.h>
#endif 
#include <fcntl.h>


TerminalWriter UI::ts;
#ifndef _WIN32
struct termios UI::ot;
#endif

std::function<void()> UI::window_resize_handle;
#ifdef _WIN32
WindowsResizeMonitor UI::wrm;
#endif 

bool UI::init() {

#ifdef _WIN32
    return true;
#else
    int fd = -1;

    if (isatty(STDOUT_FILENO))      fd = STDOUT_FILENO;
    else if (isatty(STDIN_FILENO))  fd = STDIN_FILENO;
    else if (isatty(STDERR_FILENO)) fd = STDERR_FILENO;
    tcgetattr(STDIN_FILENO, &UI::ot); 
    struct termios nt = UI::ot;
    nt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &nt);

    return (fd != -1);
#endif
}


void UI::set_window_resize_handler(std::function<void()> handle_window_resize) {
    window_resize_handle = handle_window_resize;
#ifdef _WIN32
    wrm.start();
#else
    // SIGWINCH is a signal sent when the terminal/window changes size
    // window_resize_handler will get called when that happens
    std::signal(SIGWINCH, window_resize_handler);
#endif 
}

void UI::deactivate_window_resize_handler() {
#ifdef _WIN32
    wrm.stop();
#endif 
    window_resize_handle = nullptr;
}

int UI::readchar() {
#ifdef _WIN32
    return _getch(); 
#else
    int c = getchar();
    return c;
#endif 
}

std::tuple<int,int> UI::get_terminal_size() {
#ifdef _WIN32
    HANDLE c_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(c_handle, &csbi)) {
        int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return {cols, rows};
    }
    return {80, 24}; // fallback
#else
    struct winsize w{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return { w.ws_col, w.ws_row };

    int fd = -1;

    if (isatty(STDOUT_FILENO))      fd = STDOUT_FILENO;
    else if (isatty(STDIN_FILENO))  fd = STDIN_FILENO;
    else if (isatty(STDERR_FILENO)) fd = STDERR_FILENO;

    if (fd >= 0 && ioctl(fd, TIOCGWINSZ, &w) == 0) {
        int cols = (w.ws_col > 0 ? w.ws_col : 80);
        int rows = (w.ws_row > 0 ? w.ws_row : 24);
        return {cols, rows};
    }
    const char* cols_env = std::getenv("COLUMNS");
    const char* rows_env = std::getenv("LINES");
    int cols = cols_env ? std::max(40, atoi(cols_env)) : 80;
    int rows = rows_env ? std::max(10, atoi(rows_env)) : 24;
    return {cols, rows};
#endif
}

#ifdef _WIN32
void UI::window_resize_handler() {
    if (window_resize_handle) {
        window_resize_handle();
    }
}
#else
//int sig is the signal number that triggered this function
void UI::window_resize_handler([[maybe_unused]]int sig) {
    if (window_resize_handle) {
        window_resize_handle();
    }
}
#endif

void UI::render_window(std::vector<std::string> window_info) {
    UI::ts.clear_app_output();
    for (auto&& line : window_info) {
        UI::ts.write_line(line);
    }
    ts.flush();
}

void UI::clean_window() {
    UI::ts.clear_app_output();
}

void UI::finish() {
#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &UI::ot);
#endif
}