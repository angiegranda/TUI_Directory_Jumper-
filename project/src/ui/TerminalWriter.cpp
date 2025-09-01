#include "TerminalWriter.h"

#ifdef _WIN32
/**
 * @brief Enables ANSI escape sequence processing in Windows (+10 versions).
 * @details
 * - Windows consoles do not support ANSI escape codes by default. This function 
 * modifies the console mode to allow ANSI-based cursor movement, colors, and line manipulation.
 */
void TerminalWriter::enable_ansi_on_windows() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

/**
 * @brief Constructor. 
 * @details
 * - On Windows:
 *   - Enables ANSI escape sequences using @ref enable_ansi_on_windows.
 *   - Opens an output file for writing console operations.
 *   - Initializes console handle `m_hConsole`.
 * - On POSIX:
 *   - Opens `POSIX_OUTPUT_FILE`.
 * - In both case:
 * - Sets the stream state flag `m_stream_state` to indicate whether initialization succeeded. 
 * If it didn't, then the failure will be passed to UI::init through @ref get_initialization_state 
 * and after that to controller which stops inmediatly the program.
 */ 
TerminalWriter::TerminalWriter() {
#ifdef _WIN32
    enable_ansi_on_windows();
    m_fileStream.open(WINDOWS_OUTPUT_FILE);
    if (!m_fileStream.is_open()) {
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

/**
 * @brief Returns whether the terminal writer was successfully initialized.
 * @return True if initialization succeeded, false otherwise.
 */
bool TerminalWriter::get_initialization_state() {
    return m_stream_state;
}

/**
 * @brief Destructor. Cleans up terminal output before exiting.
 * @details
 * - If lines were printed, moves the cursor up and erases them.
 * - Restores the cursor to the top after clearing.
 * - Flushes and closes the output stream.
 */
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

/**
 * @brief Writes a line of text to the terminal output stream.
 * @param msg The message to be written.
 * @note Increments the printed lines counter for proper cleanup.
 */
void TerminalWriter::write_line(const std::string& msg) {
    (*m_out) << msg << std::endl;
    m_printed_lines++;
}

/**
 * @brief Flushes the updated output stream.
 */
void TerminalWriter::flush() {
    m_out->flush();
}

/**
 * @brief Clears all previously printed lines from the terminal output.
 * @details
 * - Moves the cursor upward line by line.
 * - Erases each line.
 * - Resets the printed lines counter.
 */
void TerminalWriter::clear_app_output() {
    for (size_t i = 0; i < m_printed_lines; ++i) {
        move_cursor_up(1);
        erase_line();
    }
    m_printed_lines = 0;
    flush();
}

/**
 * @brief Moves the cursor up by a given number of lines.
 * @param lines Number of lines to move up.
 */
void TerminalWriter::move_cursor_up(size_t lines) {
    (*m_out) << MOVE_CURSOR_UP_PART1 << lines << MOVE_CURSOR_UP_PART2;
}

/**
 * @brief Moves the cursor up by a given number of lines.
 * @param lines Number of lines to move up.
 */
void TerminalWriter::erase_line() {
    (*m_out) <<ERASE_LINE_CURSOR;
}
