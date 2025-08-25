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
    if (!fileStream.is_open()) throw std::runtime_error("Failed to open CONOUT$");
    out = &fileStream;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) throw std::runtime_error("Failed to get console handle");
#else
    fileStream.open("/dev/tty");
    if (!fileStream.is_open()) throw std::runtime_error("Failed to open /dev/tty");
    out = &fileStream;
#endif
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
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    csbi.dwCursorPosition.Y -= static_cast<SHORT>(lines);
    SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
#else
    (*out) << "\x1b[" << lines << "A";
#endif
}

void TerminalWriter::erase_line() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD count;
    COORD pos = csbi.dwCursorPosition;
    FillConsoleOutputCharacter(hConsole, ' ', csbi.dwSize.X, {0, pos.Y}, &count);
    SetConsoleCursorPosition(hConsole, pos);
#else
    (*out) << "\x1b[2K\r";
#endif
}
