#ifndef _CDF_STREAM_H_
#define _CDF_STREAM_H_

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include "TerminalConstants.h"

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

class TerminalWriter {

private:

    bool m_stream_state;
    std::ofstream m_fileStream;
    std::ostream* m_out = nullptr;
    size_t m_printed_lines = 0;

#ifdef _WIN32
    HANDLE m_hConsole = nullptr;
    void enableAnsiOnWindows();
#endif

    void move_cursor_up(size_t lines);
    void erase_line();

public:

    TerminalWriter();
    ~TerminalWriter();
    void write_line(const std::string& msg);
    void flush();
    void clear_app_output();
    bool get_initialization_state();
};

#endif 

