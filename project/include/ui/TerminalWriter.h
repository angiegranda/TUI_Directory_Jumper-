/**
 * @file TerminalWriter.h
 * @author Angie Granda
 * @brief Handles writing to terminal with cross-platform support.
 * @date 2025-08-30
 * @copyright Copyright (c) 2025
 */
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

/**
 * @class TerminalWriter
 * @brief Class responsible for writing and updating terminal output.
 * @details
 * Handles opening/closing streams and rendering text dynamically, using
 * ANSI escape sequences where supported. Works on POSIX and Windows (+10)
 * terminals to simulate in-place updates.
 */

class TerminalWriter {

private:

    bool m_stream_state;
    std::ofstream m_fileStream;
    std::ostream* m_out = nullptr;
    size_t m_printed_lines = 0;

#ifdef _WIN32
    HANDLE m_hConsole = nullptr;
    void enable_ansi_on_windows();
#endif

public:

    TerminalWriter();
    ~TerminalWriter();
    void write_line(const std::string& msg);
    void flush();
    void clear_app_output();
    bool get_initialization_state();
    void move_cursor_up(size_t lines);
    void erase_line();
};

#endif 

