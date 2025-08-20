#include "UI.h"
#include <iostream>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <csignal>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <unistd.h>
#endif 




std::function<void()> UI::window_resize_handle;
#ifdef _WIN32
WindowsResizeMonitor UI::wrm;
#endif 

void UI::set_window_resize_handler(std::function<void()> handle_window_resize) {
    window_resize_handle = handle_window_resize;
#ifdef _WIN32
    wrm.start();
#else
    std::signal(SIGWINCH, window_resize_handler);
#endif 
}

void UI::deactivate_window_resize_handler() {
#ifdef _WIN32
    wrm.stop();
#endif 
    window_resize_handle = nullptr;
}

int UI::readchar() {
#ifdef _WIN32
    return _getch(); 
#else
    struct termios ot; 
    tcgetattr(STDIN_FILENO, &ot); 
    struct termios nt = ot;
    nt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &nt);
    int c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &ot);
    return c;
#endif 
}


std::tuple<int, int> UI::get_terminal_size() {
#ifdef _WIN32
    HANDLE c_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(c_handle, &csbi)) {
        int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return std::make_tuple(cols, rows);
    }
    // TODO: handle error
    throw "could not read the terminal size";
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return std::make_tuple(w.ws_col, w.ws_row);
#endif // _WIN32
}


#ifdef _WIN32
void UI::window_resize_handler() {
    if (window_resize_handle) {
        window_resize_handle();
    }
}
#else
void UI::window_resize_handler([[maybe_unused]]int sig) {
    if (window_resize_handle) {
        window_resize_handle();
    }
}
#endif




std::vector<std::tuple<std::string, bool, bool>> UI::preprocess_contents(
    std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t> contents, 
    std::size_t row_width, std::size_t total_lines)
{
    std::vector<std::tuple<std::string, bool, bool>> processed_output;

    std::shared_ptr<Buffer> buffer = std::get<0>(contents);
    bool has_selected_dir = std::get<1>(contents);
    std::size_t selected_dir_index = std::get<2>(contents);

    std::size_t start_pos_display = selected_dir_index >= total_lines
        ? selected_dir_index - total_lines + 1
        : 0;

    std::size_t end_pos_display = selected_dir_index >= total_lines
        ? selected_dir_index
        : total_lines - 1;

    std::size_t curr_pos = 0;
    std::string empty_line(row_width, ' ');  

    if (buffer->has_directories()) {
        const auto& directories = buffer->get_directories();
        std::size_t dir_end_pos = std::min(directories.size() - 1, end_pos_display);

        for (curr_pos = start_pos_display; curr_pos <= dir_end_pos; ++curr_pos) {
            bool highlighted = has_selected_dir && curr_pos == selected_dir_index;
            std::string name;
            if (directories[curr_pos].size() > row_width) {
                name = directories[curr_pos].substr(0, row_width - 3) + "...";
            } 
            else {
                //std::cout << directories[curr_pos] << " additonal: -" << ' ' * (row_width - directories[curr_pos].size()) << "-" << std::endl;
                std::string additional(row_width - directories[curr_pos].size(), ' ');
                name = directories[curr_pos] + additional;
                //std::cout << "Name: -"<< name << "-" << std::endl;
            }
            processed_output.emplace_back(name, true, highlighted);
        }
    }

    if (curr_pos < total_lines && !buffer->get_files().empty()) {
        const auto& files = buffer->get_files();
        std::size_t lines_remaining = total_lines - (curr_pos + 1);
        std::size_t end_files_pos = std::min(files.size(), lines_remaining);

        for (std::size_t i = 0; i < end_files_pos; ++i, ++curr_pos) {
            std::string name;
            if (files[i].size() > row_width) {
                name = files[i].substr(0, row_width - 3) + "...";
            } else {
                std::string additional(row_width - files[i].size(), ' ');
                name = files[i] + additional;
            }
            processed_output.emplace_back(name, false, false);
        }
    }
    while (curr_pos < total_lines) {
        processed_output.emplace_back(empty_line, false, false);
        ++curr_pos;
    }
    return processed_output;
}

std::string UI::string_format(
    const std::tuple<std::string, bool, bool>& item) {
    const std::string& text = std::get<0>(item);
    bool bold = std::get<1>(item);
    bool highlighted = std::get<2>(item);

    std::string output;

    if (bold) {
        output += BOLD_ON;
    }
    if (highlighted) {
        output += HIGHLIGHTED_BACKGROUND;
        output += LETTER_COLOR_HIGHLIGHTED;
    } else {
        output += LETTER_COLOR_NORMAL;
        output += BACKGROUND;
    }

    output += text;
    output += RESET;

    return output;
}

void UI::render_two_windows(
    std::vector<std::tuple<std::string, bool, bool>> left_contents, 
    std::vector<std::tuple<std::string, bool, bool>> right_contents,
    int total_lines
){
    for (int i = 0 ; i < total_lines; ++i) {
        std::cout << UI::string_format(left_contents[i]) 
        << std::string(' ', LINE_INTERVAL) 
        << UI::string_format(right_contents[i]) << "\n";
    }

}

void UI::render_three_windows(
    std::vector<std::tuple<std::string, bool, bool>> left_contents, 
    std::vector<std::tuple<std::string, bool, bool>> middle_contents,
    std::vector<std::tuple<std::string, bool, bool>> right_contents,
    int total_lines
){
    for (int i = 0 ; i < total_lines; ++i) {
        std::cout << UI::string_format(left_contents[i]) 
        << std::string(' ', LINE_INTERVAL) 
        << UI::string_format(middle_contents[i]) 
        << std::string(' ', LINE_INTERVAL) 
        << UI::string_format(right_contents[i]) << "\n";
    }
}

void UI::render_window(
    std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t> left_contents, 
    std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t> middle_contents, 
    std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t> right_contents
) {
    //enableAnsiOnWindows();
    std::cout << CURSOR_TOP_LEFT << CLEAR_BELOW;
    auto [width, height] = UI::get_terminal_size();
    int total_lines = height-5;

    if (std::get<0>(left_contents) == nullptr) {
        int row_width = (width - LINE_INTERVAL) / TWO_COLUMNS; 
        UI::render_two_windows(
            UI::preprocess_contents(middle_contents, row_width, total_lines), 
            UI::preprocess_contents(right_contents, row_width, total_lines),
            total_lines);
    }
    else if (std::get<0>(right_contents) == nullptr){
        int row_width = (width - LINE_INTERVAL) / TWO_COLUMNS; 
        UI::render_two_windows(
        UI::preprocess_contents(left_contents, row_width, total_lines), 
        UI::preprocess_contents(middle_contents, row_width, total_lines),
        total_lines);
    }
    else {
        int row_width = (width - 2*LINE_INTERVAL) / THREE_COLUMNS; 
        UI::render_three_windows(
        UI::preprocess_contents(left_contents, row_width, total_lines), 
        UI::preprocess_contents(middle_contents, row_width, total_lines), 
        UI::preprocess_contents(right_contents, row_width, total_lines),
        total_lines);
    }
}


// FEATURE 
// Add a feature in which when we press some key we navegate in the child repostory only of files just to lookfor what we want, whis highlited could be done in yellow oe something similar 