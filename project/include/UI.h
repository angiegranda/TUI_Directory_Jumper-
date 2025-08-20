#ifndef _CFD_UI_H_
#define _CFD_UI_H_
#include "UI.h"
#include "Buffer.h"
#include <functional>
#include <tuple>
#include <optional>
#include <memory>
#include <tuple>

#ifdef _WIN32
    #include "WindowsResizeMonitor.h"
#endif 

constexpr char MOVE_CURSOR_TOP_LEFT[] = "\033[H";
constexpr char CLEAR_SCREEN_BELOW[]   = "\033[J"; // clear from cursor down
// ANSI control sequences
// constexpr char ENTER_ALT_SCREEN[] = "\033[?1049h";
// constexpr char EXIT_ALT_SCREEN[]  = "\033[?1049l";
// constexpr char HIDE_CURSOR[]      = "\033[?25l";
// constexpr char SHOW_CURSOR[]      = "\033[?25h";
constexpr char CURSOR_TOP_LEFT[]  = "\033[H";
constexpr char CLEAR_BELOW[]      = "\033[J";

// ANSI COLOR CODES
constexpr char DEFAULT_COLOR[] = "\033[0m";
constexpr char RED_COLOR[] = "\033[31m";
constexpr char GREEN_COLOR[] = "\033[32m";
constexpr char YELLOW_COLOR[] = "\033[33m";
constexpr char BLUE_COLOR[] = "\033[34m";
constexpr char MAGENTA_COLOR[] = "\033[35m";
constexpr char CYAN_COLOR[] = "\033[36m";
constexpr char WHITE_COLOR[] = "\033[37m";

// ANSI BACKGROUND COLOR CODES
constexpr char BLACK_BG_COLOR[] = "\033[40m";
constexpr char RED_BG_COLOR[] = "\033[41m";
constexpr char GREEN_BG_COLOR[] = "\033[42m";
constexpr char YELLOW_BG_COLOR[] = "\033[43m";
constexpr char BLUE_BG_COLOR[] = "\033[44m";
constexpr char MAGENTA_BG_COLOR[] = "\033[45m";
constexpr char CYAN_BG_COLOR[] = "\033[46m";
constexpr char WHITE_BG_COLOR[] = "\033[47m";

// STRING FORMAT 

constexpr char BOLD_ON[]  = "\033[1m";
constexpr char RESET[]    = "\033[0m";

constexpr char HIGHLIGHTED_BACKGROUND[]    = "\033[46m"; // Cyan background
constexpr char LETTER_COLOR_HIGHLIGHTED[]  = "\033[30m"; // Black text (corrected from "\033[40m")

// (terminal default)
constexpr char BACKGROUND[]               = "";          // Leave empty for terminal default
constexpr char LETTER_COLOR_NORMAL[]      = "";          // Leave empty for terminal default



constexpr int LINE_INTERVAL = 4; 
constexpr int THREE_COLUMNS = 3;
constexpr int TWO_COLUMNS = 2;
constexpr int WORD_WIDTH = 2;



class UI {

private:
    static std::function<void()> window_resize_handle;
    static std::tuple<int, int> get_terminal_size();
    static std::vector<std::tuple<std::string, bool, bool>> preprocess_contents(std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t> contents, std::size_t row_width, std::size_t total_lines);
    static std::string string_format(const std::tuple<std::string, bool, bool>& content);
    

#ifdef _WIN32
    friend WindowsResizeMonitor;
    static WindowsResizeMonitor wrm;
    static void window_resize_handler();
#else
    static void window_resize_handler(int sig);
#endif

public:

    // KEEP  
    static void set_window_resize_handler(std::function<void()> handle_window_resize);
    static void deactivate_window_resize_handler();
    static int readchar();

    // DISPLAY

    static void print_column(std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t> data, 
        std::size_t height, std::size_t width_start_pos, std::size_t width);

    static void render_two_windows(
        std::vector<std::tuple<std::string, bool, bool>> left_contents, 
        std::vector<std::tuple<std::string, bool, bool>> right_contents,
        int total_lines
    );
    static void render_three_windows(
        std::vector<std::tuple<std::string, bool, bool>> left_contents, 
        std::vector<std::tuple<std::string, bool, bool>> middle_contents,
        std::vector<std::tuple<std::string, bool, bool>> right_contents,
        int total_lines
    );

    static void render_window(
        std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t> left_contents, 
        std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t> middle_contents, 
        std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t> right_contents
    );
};


#endif 
