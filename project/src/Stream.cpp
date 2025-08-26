#include "Stream.h"

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
    fileStream.open("CONOUT$");
    if (!fileStream.is_open()) {
        stream_state = false;
    }
    else {
        out = &fileStream;
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole == INVALID_HANDLE_VALUE) {
            stream_state = false;
        }
        else {
            stream_state = true;
        }
    }
#else
    fileStream.open("/dev/tty");
    if (!fileStream.is_open()) {
        stream_state = false;
    }
    else {
        out = &fileStream;
        stream_state = true;
    }
#endif
}

bool TerminalWriter::get_initialization_state() {
    return stream_state;
}

TerminalWriter::~TerminalWriter() {
    if (printed_lines == 0) return;
    move_cursor_up(printed_lines);
    for (size_t i = 0; i < printed_lines; ++i) {
        erase_line();        
        if (i + 1 < printed_lines) {
            (*out) << "\x1b[1B";  
        }
    }
    if (printed_lines > 1) { 
        move_cursor_up(printed_lines);
    }
    printed_lines = 0;
    flush();
    if (fileStream.is_open()) fileStream.close();
}

void TerminalWriter::write_line(const std::string& msg) {
    (*out) << msg << std::endl;
    printed_lines++;
}

void TerminalWriter::flush() {
    out->flush();
}

void TerminalWriter::clear_app_output() {
    for (size_t i = 0; i < printed_lines; ++i) {
        move_cursor_up(1);
        erase_line();
    }
    printed_lines = 0;
    flush();
}


void TerminalWriter::move_cursor_up(size_t lines) {
    (*out) << "\x1b[" << lines << "A";
}

void TerminalWriter::erase_line() {
    (*out) << "\x1b[2K\r";
}
