#ifndef _CFD_CONSTANTS_H
#define _CFD_CONSTANTS_H
#include <filesystem>

// ALIAS 

namespace fs = std::filesystem; 
using Metrics_Tuple = std::tuple<std::size_t, std::size_t, std::size_t>;

// CONTROLLER 

constexpr std::size_t MAX_QUICK_ACCESS_ITEMS = 30;
constexpr char QUICK_ACCESS_PROJECT_FOLDER[] = "tcd";
constexpr char QUICK_ACCESS_FILE[] = "quick_access.csv";


// EXPECTED ENVIRONMENT VARIABLES
constexpr char GLOBAL_ENV_PATH_POSIX1[] = "XDG_DATA_HOME";
constexpr char GLOBAL_ENV_PATH_POSIX2[] = "HOME";
constexpr char GLOBAL_ENV_PATH_WIND[] = "APPDATA";


// FILE CONTENTS 

constexpr char ERROR_OPEN_FILE[] = "Error: Could not open file ";
constexpr char EMPTY_FILE[] = "[Empty File]";
constexpr char BINARY_FILE[] = "[Binary File]";

// CHARS 

constexpr char NEW_LINE = '\n';
constexpr char CARRIAGE_RETURN = '\r';
constexpr char TAB = '\t';
constexpr char CSV_SEPARATOR = ',';
constexpr char STAR = '*';
constexpr char SPACE = ' '; 

// NAVIGATION HEADER AND FOOTER 

constexpr char NAVIGATION_HEADER[] = "==== Navigation ===="; 
constexpr char NAVIGATION_FOOTER1[] = "[k]-Up  [j]-Down  [h]-Backward  [l]-Forward"; 
constexpr char NAVIGATION_FOOTER2[] = "[q]-Quit [s]-Select [c]-Change to Quick Access";
constexpr char EMPTY_DIR_MESSAGE1[] = "Empty directory";
constexpr char EMPTY_DIR_MESSAGE2[] = "Please move backward";

// QUICKACCESS HEADER AND FOOTER

constexpr char QUICKACCESS_HEADER[] = "==== Quick Access ====";
constexpr char QUICKACCESS_FOOTER1[] = "[j]-Down  [k]-Up  [x]-Clean";
constexpr char QUICKACCESS_FOOTER2[] = "[q]-Quit  [s]-Select [c]-Change to Navigation";

// QUICKACCESS 

constexpr std::size_t PADDING = 4;
constexpr double LAMBDA = 0.8;
#ifdef _WIN32
    constexpr char SLASH[] = "\\";
#else 
    constexpr char SLASH[] = "/";
#endif
constexpr char DIR_SHORTCUT[] = "..";
constexpr std::size_t QUICKACCESS_RESERVED_LINES = 7; // 1 header, 2 footer, 2 between, 2 for correct visualization


// NAVIGATION  

constexpr std::size_t NAVIGATION_RESERVED_LINES = 7;
constexpr std::size_t BETWEEN_COLUMNS_SPACE = 2;
constexpr std::size_t NAVIGATION_MIN_LINES = 10;
constexpr double BINARY_HEURISTIC = 0.1;
constexpr char DOTS[] = "...";
constexpr std::size_t MIN_ASCII = 32;
constexpr std::size_t MAX_ASCII = 127;
#endif