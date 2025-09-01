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
/**
 * @brief Terminal stream through special files. They depend on the operative system.
 */
TerminalWriter UI::ts;
#ifndef _WIN32
/**
 * @brief It stores terminal attributes for Mac/Linux
 */
struct termios UI::ot;
#else
WindowsResizeMonitor UI::wrm;
#endif



/**
 * @brief Initialize the terminal for extracting input mode and stream .
 * @details
 * On Mac/Linux:
 * - Verifies if input is from a TTY.
 * - Disables canonical mode and echoing.
 * - Saves original terminal attributes.
 * On Windows:
 * - No setup needed
 * @return true if initialization succeeded, false otherwise.
 */
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

/**
 * @brief Set a handler for terminal window resize events.
 * @param handle_window_resize Function to be called when terminal size changes.
 * On Windows:
 * - Starts a background resize monitor.
 * On UNIX:
 * - Registers a SIGWINCH signal handler.
 */
void UI::set_window_resize_handler(std::function<void()> handle_window_resize) {
    window_resize_handle = handle_window_resize;
#ifdef _WIN32
    wrm.start();
#else
    std::signal(SIGWINCH, window_resize_handler);
#endif 
}

/**
 * @brief Disable the terminal resize handler.
 * @details
 * On Windows:
 * - Stops the resize monitor.
 */
void UI::deactivate_window_resize_handler() {
#ifdef _WIN32
    wrm.stop();
#endif 
    window_resize_handle = nullptr;
}

/**
 * @brief Read a single character from terminal input.
 * @details
 * On Windows:
 * - Uses `_getch()` 
 * On UNIX:
 * - Uses `getchar()`
 * @return ASCII code of the character read.
 */
int UI::readchar() {
#ifdef _WIN32
    return _getch(); 
#else
    int c = getchar();
    return c;
#endif 
}

/**
 * @brief Get the current terminal window size.
 * @details
 * On Windows:
 * - Uses constants `WINDOWS_TERMINAL_WIDTH` and `WINDOWS_TERMINAL_HEIGHT` 
 * that can be modified in TerminalConstants.h
 * @return std::tuple<int,int> containing {columns, rows}.
 * @see TerminalConstants.h
 */
std::tuple<int,int> UI::get_terminal_size() {
#ifdef _WIN32
    return {WINDOWS_TERMINAL_WIDTH, WINDOWS_TERMINAL_HEIGHT};
#else 
    struct winsize w{};
    ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
    return { w.ws_col, w.ws_row };
#endif
}


/**
 * @brief Resize event handler.
 * @details
 * - On Windows: Invokes the registered `window_resize_handle()`.
 * - On UNIX: Invokes the registered `window_resize_handle(int)`.
 * @param sig Signal number, it is ignored.
 */
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

/**
 * @brief Static function that takes a vector of strings and uses the special
 * file stream to print in the terminal.
 * @param window_info Vector of strings, each representing a line of output.
 */
void UI::render_window(std::vector<std::string> window_info) {
    UI::ts.clear_app_output();
    for (auto&& line : window_info) {
        UI::ts.write_line(line);
    }
    ts.flush();
}

/**
 * @brief Clear the terminal output.
 */
void UI::clean_window() {
    UI::ts.clear_app_output();
}


/**
 * @brief Restore the terminal to its original state.
 * @details
 * On UNIX:
 * - Restores saved terminal attributes: `ICANON`, `ECHO`.
 */
void UI::finish() {
#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &UI::ot);
#endif
}
