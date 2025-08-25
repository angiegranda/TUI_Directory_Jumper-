#ifndef _CFD_UI_H_
#define _CFD_UI_H_
#include "Buffer.h"
#include "Stream.h"
#include <functional>
#include <tuple>
#include <fstream>

#ifdef _WIN32
    #include "WindowsResizeMonitor.h"
#endif 


class UI {

private:
    static std::function<void()> window_resize_handle;
#ifdef _WIN32
    friend WindowsResizeMonitor;
    static WindowsResizeMonitor wrm;
    static void window_resize_handler();
#else
    static void window_resize_handler(int sig);
#endif

public:

    static TerminalWriter ts;
    static std::tuple<int, int> get_terminal_size();
    static void set_window_resize_handler(std::function<void()> handle_window_resize);
    static void deactivate_window_resize_handler();
    static int readchar();
    static void render_window(std::vector<std::string> window_info);
    static void clean_window();
    static void start_resize_monitor();
};

#endif 




