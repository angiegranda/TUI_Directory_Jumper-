#ifndef _CFD_CONTROLLER_H_
#define _CFD_CONTROLLER_H_

#include "Alias.h"
#include "Explorer.h"


/////////// TESTING: WORKING BUT SHOULD BE IN IU

#ifdef _WIN32
#include <windows.h>
void enableAnsiOnWindows() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    // Enable ANSI escape processing
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#else
void enableAnsiOnWindows() {} 
#endif


constexpr char ENTER_ALT_SCREEN[] = "\033[?1049h";
constexpr char EXIT_ALT_SCREEN[]  = "\033[?1049l";
constexpr char HIDE_CURSOR[]      = "\033[?25l";
constexpr char SHOW_CURSOR[]      = "\033[?25h";



/////////

class Controller {

private:
    Explorer m_explorer;
    void update_pos();
    void init();
    void program_loop();
    void finish();
    void on_window_resize() const;

public:
    Controller(fs::path current_path);
    void run();
};

#endif 
