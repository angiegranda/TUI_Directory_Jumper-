#include "TerminalWriter.h"

#ifdef _WIN32
void TerminalWriter::enableAnsiOnWindows() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

TerminalWriter::TerminalWriter() {
#ifdef _WIN32
    enableAnsiOnWindows();
    m_fileStream.open(WINDOWS_OUTPUT_FILE);
    if (!fileStream.is_open()) {
        m_stream_state = false;
    }
    else {
        m_out = &m_fileStream;
        m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (m_hConsole == INVALID_HANDLE_VALUE) {
            m_stream_state = false;
        }
        else {
            m_stream_state = true;
        }
    }
#else
    m_fileStream.open(POSIX_OUTPUT_FILE);
    if (!m_fileStream.is_open()) {
        m_stream_state = false;
    }
    else {
        m_out = &m_fileStream;
        m_stream_state = true;
    }
#endif
}

bool TerminalWriter::get_initialization_state() {
    return m_stream_state;
}

TerminalWriter::~TerminalWriter() {
    if (m_printed_lines == 0) return;
    move_cursor_up(m_printed_lines);
    for (size_t i = 0; i < m_printed_lines; ++i) {
        erase_line();        
        if (i + 1 < m_printed_lines) {
            (*m_out) << MOVE_CURSOR_DOWN;  
        }
    }
    if (m_printed_lines > 1) { 
        move_cursor_up(m_printed_lines);
    }
    m_printed_lines = 0;
    flush();
    if (m_fileStream.is_open()) m_fileStream.close();
}

void TerminalWriter::write_line(const std::string& msg) {
    (*m_out) << msg << std::endl;
    m_printed_lines++;
}

void TerminalWriter::flush() {
    m_out->flush();
}

void TerminalWriter::clear_app_output() {
    for (size_t i = 0; i < m_printed_lines; ++i) {
        move_cursor_up(1);
        erase_line();
    }
    m_printed_lines = 0;
    flush();
}

void TerminalWriter::move_cursor_up(size_t lines) {
    (*m_out) << MOVE_CURSOR_UP_PART1 << lines << MOVE_CURSOR_UP_PART2;
}

void TerminalWriter::erase_line() {
    (*m_out) << ERASE_LINE_CURSOR;
}
