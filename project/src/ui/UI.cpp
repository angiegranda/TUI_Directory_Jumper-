// 
#include "UI.h"
#ifdef _WIN32
    #include <conio.h> // _getch()
    #define NOMINMAX
    #include <windows.h> // AttachConsole, GetStdHandle, CONSOLE_SCREEN_BUFFER_INFO, HANDLE, INVALID_HANDLE_VALUE
#else
    #include <csignal> // std::signal
    #include <sys/ioctl.h> // ioctl
    #include <termios.h> // termios, tcgetattr, tcsetattr
    #include <unistd.h> // isatty, STDIN_FILENO
#endif 

std::function<void()> UI::window_resize_handle;
TerminalWriter UI::ts;
#ifndef _WIN32
struct termios UI::ot;
#else
WindowsResizeMonitor UI::wrm;
#endif


bool UI::init() {
#ifdef _WIN32
    return true;
#else
    if (!isatty(STDIN_FILENO)) {
        return false;
    }
    tcgetattr(STDIN_FILENO, &UI::ot); 
    struct termios nt = UI::ot;
    nt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &nt);
    return UI::ts.get_initialization_state();
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
    AttachConsole(ATTACH_PARENT_PROCESS); 
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return { 60, 25 };

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return { 90, 30};
    }
    int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return { cols, rows };
#else 
    struct winsize w{};
    ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
    return { w.ws_col, w.ws_row };
#endif
}

#ifdef _WIN32
void UI::window_resize_handler() {
    if (window_resize_handle) {
        window_resize_handle();
    }
}
#else
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
