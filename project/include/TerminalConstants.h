/**
 * @file TerminalConstants.h
 * @author Angie Granda (angiegrandauni@gmail.com)
 * @brief This file defines constants for terminal output and text color formatting.
 * 
 * @date 2025-08-30
 * @copyright Copyright (c) 2025
 */

#ifndef _CFD_TERMINAL_CONSTANTS_H_
#define _CFD_TERMINAL_CONSTANTS_H_
#include <iostream>


/**
 * @defgroup terminal_output_files 
 * @brief Output special files that will print the contents to the terminal.
 * @{
 */
constexpr char WINDOWS_OUTPUT_FILE[] = "CONOUT$";
constexpr char POSIX_OUTPUT_FILE[] = "/dev/tty";
/// @}

/**
 * @defgroup terminal_size_settings 
 * @{
 */
/// @brief For Windows device: Terminal width constant can be changed by the user 
constexpr std::size_t WINDOWS_TERMINAL_WIDTH = 90; 
/// @brief For Windows device: Terminal height constant can be changed by the user 
constexpr std::size_t WINDOWS_TERMINAL_HEIGHT = 30;
/// @brief  Minimum terminal width such that the program works. Less is insufficient. 
constexpr std::size_t MIN_TERMINAL_WIDTH = 40;
/// @brief  Minimum terminal height such that the program works. Less is insufficient. 
constexpr std::size_t MIN_TERMINAL_HEIGHT = 14;
/// @}

/**
 * @defgroup ansi_codes 
 * @{
 */
constexpr char BOLD_ON[]  = "\033[1m";
constexpr char RESET[]    = "\033[0m";
constexpr char MAGENTA_TEXT_COLOR[] = "\033[35m";
constexpr char WHITE_TEXT_COLOR[] = "\033[37m";
constexpr char BLACK_TEXT_COLOR[] = "\033[30m";
constexpr char PALE_GREEN_HIGHLIGHT[] = "\033[102m";
constexpr char MOVE_CURSOR_DOWN[] = "\x1b[1B";
constexpr char MOVE_CURSOR_UP_PART1[] = "\x1b[";
constexpr char MOVE_CURSOR_UP_PART2[] = "A";
constexpr char ERASE_LINE_CURSOR[] = "\x1b[2K\r";
/// @}
#endif