/**
 * @file Constants.h
 * @author Angie Granda
 * @brief Constants and configuration values for Controller, Quick Access and Navigation.
 *
 * This file defines constants used across the navigation and quick access views, 
 * environment variables for locating configuration files, and general display-related 
 * settings for navigation and quick access views.
 * 
 * @date 2025-08-30
 * @copyright Copyright (c) 2025
 */

#ifndef _CFD_CONSTANTS_H
#define _CFD_CONSTANTS_H
#include <filesystem>

/** @defgroup namespaces_alias 
 *  @brief Type aliases and namespace shortcuts.
 *  @{
 */
namespace fs = std::filesystem; 
using Metrics_Tuple = std::tuple<std::size_t, std::size_t, std::size_t>;
/// @}



/** @defgroup env_variables
 *  @brief Paths where the project folder 'tcd' and the quickaccess data file will be stored.
 *  @{
 */
constexpr char  GLOBAL_ENV_PATH_POSIX1[] = "XDG_DATA_HOME";
constexpr char  GLOBAL_ENV_PATH_POSIX2[] = "HOME";
constexpr char  GLOBAL_ENV_PATH_WIND[] = "APPDATA";
/// @}


/** @defgroup general_chars
 *  @brief Paths where the project folder 'tcd' and the quickaccess data file will be stored.
 *  @{
 */
constexpr char NEW_LINE = '\n';
constexpr char CARRIAGE_RETURN = '\r';
constexpr char TAB = '\t';
constexpr char CSV_SEPARATOR = ',';
constexpr char STAR = '*';
constexpr char SPACE = ' '; 
/// @}



/** @defgroup quick_access
 *  @brief Constants related to QuickAccess.h and QuickAccessView.h.
 * Quick Access view dispay settings.
 *  @{
 */
/// Maximum number of paths displayed in the Quick Access view. 
constexpr std::size_t MAX_QUICK_ACCESS_ITEMS = 30;
/// Folder under $HOME (POSIX) or %APPDATA% (Windows) where Quick Access data is stored.
constexpr char QUICK_ACCESS_PROJECT_FOLDER[] = "tcd";
/// File used to store Quick Access data (paths and their frequencies of being chosen).
constexpr char QUICK_ACCESS_FILE[] = "quick_access.csv";
/// ANSI escape sequence that clears the screen (\033[2J) and moves the cursor to the top-left corner (\033[H).
/// Useful to avoid leftover characters when printing new output.
/// Supported natively on POSIX systems (Linux/macOS). On Windows, supported in newer terminals (10+).
constexpr char CLEAR_WINDOWS_COMMAND_LINE[] = "\033[2J\033[H";

/// Quick Access header text.
constexpr char QUICKACCESS_HEADER[] = "==== Quick Access ====";
/// Quick Access 1st footer actions guide.
constexpr char QUICKACCESS_FOOTER1[] = "[j]-Down  [k]-Up  [x]-Clean";
/// Quick Access 2nd footer actions guide.
constexpr char QUICKACCESS_FOOTER2[] = "[q]-Quit  [s]-Select [c]-Change to Navigation";

/// Padding for each displayed path in Quick Access view (2 spaces, a '*', and one trailing space).
constexpr std::size_t PADDING = 4;
/// Exponential decay factor for path importance weighting in Quick Access.
constexpr double LAMBDA = 0.8;
/// Directory slash separator.
#ifdef _WIN32
    constexpr char SLASH[] = "\\";
#else 
    constexpr char SLASH[] = "/";
#endif
/// Dots string used to indicate truncated output.
constexpr char DIR_SHORTCUT[] = "..";
/// Quick Access window number of lines reserved for header, footer and padding.
constexpr std::size_t QUICKACCESS_RESERVED_LINES = 7; // 1 header, 2 footer, 2 between, 2 for correct visualization
/// @}




/** @defgroup navegation
 * @brief Constants related to NavigationView.h.
 * File visualization types, text processing constants and Navigation view display settings. 
 * @{
 */ 
/// @brief Navigation window number of lines reserved for header, footer and paddings.
constexpr std::size_t NAVIGATION_RESERVED_LINES = 7;
/// @brief Amount of spaces between columns in Navigation view. 
constexpr std::size_t BETWEEN_COLUMNS_SPACE = 2;
/// @brief Minimum amount of lines for Navigation view display which includes the reserved lines. 
constexpr std::size_t NAVIGATION_MIN_LINES = 10;
/// @brief Minimum percentage of non readable characters in a file for it to be considered binary.
constexpr double BINARY_HEURISTIC = 0.1;
constexpr char DOTS[] = "...";
/// @brief Minimum ASCII readable character
constexpr std::size_t MIN_ASCII = 32;
/// @brief Maximum ASCII readable character
constexpr std::size_t MAX_ASCII = 127;

/// @brief Navigation header text.
constexpr char NAVIGATION_HEADER[] = "==== Navigation ===="; 
/// @brief Navigation 1st footer actions guide.
constexpr char NAVIGATION_FOOTER1[] = "[k]-Up  [j]-Down  [h]-Backward  [l]-Forward"; 
/// @brief Navigation 2nd footer actions guide.
constexpr char NAVIGATION_FOOTER2[] = "[q]-Quit [s]-Select [c]-Change to Quick Access";
/// @brief Navigation error 1st message when calling the program from an empty directory
constexpr char EMPTY_DIR_MESSAGE1[] = "Empty directory";
/// @brief Navigation error 2nd message when calling the program from an empty directory
constexpr char EMPTY_DIR_MESSAGE2[] = "Please move backward";
constexpr char ERROR_OPEN_FILE[] = "Error: Could not open file ";
constexpr char EMPTY_FILE[] = "[Empty File]";
constexpr char BINARY_FILE[] = "[Binary File]";
/// @}

#endif