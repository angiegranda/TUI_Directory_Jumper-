#ifndef _CFD_TERMINAL_CONSTANTS_H_
#define _CFD_TERMINAL_CONSTANTS_H_
#include <iostream>


// TERMINAL OUTPUT FILES 

constexpr char WINDOWS_OUTPUT_FILE[] = "CONOUT$";
constexpr char POSIX_OUTPUT_FILE[] = "/dev/tty";

constexpr std::size_t MIN_TERMINAL_WIDTH = 40;
constexpr std::size_t MIN_TERMINAL_HEIGHT = 14;

constexpr char BOLD_ON[]  = "\033[1m";
constexpr char RESET[]    = "\033[0m";

// --------------- COLORS FOR TEXT ------------------

// FOLDERS
constexpr char MAGENTA_TEXT_COLOR[] = "\033[35m";

// TEXT, FILE NAMES WHEN NOT SELECTED, HEADER AND FOOTER 
constexpr char WHITE_TEXT_COLOR[] = "\033[37m";

// FILE NAMES WHEN SELECTED 
constexpr char BLACK_TEXT_COLOR[] = "\033[30m";

// --------------- HIGHLIGHTS ------------------

constexpr char PALE_GREEN_HIGHLIGHT[] = "\033[102m";

// --------------- CURSORS CODES --------------- 


constexpr char MOVE_CURSOR_DOWN[] = "\x1b[1B";
constexpr char MOVE_CURSOR_UP_PART1[] = "\x1b[";
constexpr char MOVE_CURSOR_UP_PART2[] = "A";
constexpr char ERASE_LINE_CURSOR[] = "\x1b[2K\r";

#endif