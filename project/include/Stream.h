#ifndef _CDF_STREAM_H_
#define _CDF_STREAM_H_

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

class TerminalWriter {
public:
    TerminalWriter();
    ~TerminalWriter();
    void write_line(const std::string& msg);
    void flush();
    void clear_app_output();
private:
    std::ofstream fileStream;
    std::ostream* out = nullptr;
    size_t printed_lines = 0;
    void move_cursor_up(size_t lines);
    void erase_line();
#ifdef _WIN32
    void enableAnsiOnWindows();
    HANDLE hConsole = nullptr;
    COORD start_pos{};  // 2D coordinate in the console (column X and row Y).
#endif

};


// `\x1b[2K`: Clear entire current line                          |
// `\x1b[<n>A`: Move cursor up by `n` lines                        |
// `\x1b[<n>B`: Move cursor down by `n` lines                      |
// `\r`: Return to the start of the current line (column 0) 

#endif 

