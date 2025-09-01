/**
 * @file UI.h
 * @author Angie Granda
 * @brief Static class handling terminal input, output, and window resizing.
 * @date 2025-08-30
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef _CFD_UI_H_
#define _CFD_UI_H_
#include "TerminalWriter.h"
#include <functional>
#include <tuple>

#ifdef _WIN32
    #include "WindowsResizeMonitor.h"
#endif 

/**
 * @class UI
 * @brief Controls the input and output and the resize of the terminal window.
 * @details
 * - Initialization `init` and cleanup `finish`.
 * - Reading single characters from terminal input `readchar`.
 * - Querying and updating terminal size.
 * - Setting custom resize handler for Windows.
 * - Starting/stopping monitoring threads for Windows terminal resize events.
 * @note On Windows, it uses `conio.h`, `windows.h`, and a custom `WindowsResizeMonitor`.
 * @note On UNIX-like systems, it uses `termios`, `ioctl`, and `SIGWINCH` signals.
 * @see TerminalWriter
 * @see WindowsResizeMonitor
 */

class UI {

private:
    static std::function<void()> window_resize_handle;
    static TerminalWriter ts;
#ifdef _WIN32
    friend WindowsResizeMonitor;
    static WindowsResizeMonitor wrm;
    static void window_resize_handler();
#else
    static struct termios ot; 
    static void window_resize_handler(int sig);
#endif

public:

    static bool init();
    static std::tuple<int, int> get_terminal_size();
    static void set_window_resize_handler(std::function<void()> handle_window_resize);
    static void deactivate_window_resize_handler();
    static int readchar();
    static void render_window(std::vector<std::string> window_info);
    static void clean_window();
    static void start_resize_monitor();
    static void finish();
};

#endif 




