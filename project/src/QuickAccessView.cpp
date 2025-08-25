#include "QuickAccessView.h"
#include <sstream>
#include <stack>
#include <cstring>

std::string QuickAccessView::path_length_constraints(const std::string& path, const std::size_t width) {
    if (path.size() < width) {
        return path;
    }
    std::stack<std::string> stack_;
    fs::path path_(path);
    std::size_t total_chars = 0;
    bool is_enough = false;
    while (!is_enough && !path_.empty())  {
        std::string dir = path_.filename().string();
        if ((dir.size() + total_chars + 1) < (width - std::strlen(DIR_SHORTCUT))) {
            stack_.push(dir);
            total_chars += dir.size() + 1;
            path_ = path_.parent_path();
        } else {
            is_enough = true;
        }
    }
    std::string line;
    line.reserve(width);
    line.append(DIR_SHORTCUT);
    while (!stack_.empty()) {
        line.append(SLASH);
        line.append(stack_.top());
        stack_.pop();
    }
    line.append(std::string((width < line.size() ? 0 : width - line.size()),' '));
    return line;
}

std::string QuickAccessView::get_header_or_footer(const char* text, const std::size_t width) {
    std::size_t remaining_spaces = width > std::strlen(text) ? width - std::strlen(text) : 0;
    std::size_t corners_spaces = remaining_spaces / 2;
    std::size_t reminder = remaining_spaces % 2;
    std::string line;
    line.reserve(width);
    line.append(WHITE_TEXT_COLOR); line.append(BOLD_ON);
    line.append(std::string(corners_spaces, SPACE) + text + std::string(corners_spaces + reminder, SPACE));
    line.append(RESET);
    return line;
}

void QuickAccessView::render_window(std::vector<std::string> paths_list, std::size_t pos)  {
    std::vector<std::string> contents_to_display;
    auto [width_, height_] = UI::get_terminal_size();
    std::size_t width  = std::max<std::size_t>(MIN_TERMINAL_WIDTH, width_);
    std::size_t height = std::max<std::size_t>(MIN_TERMINAL_HEIGHT, height_);

    contents_to_display.emplace_back(get_header_or_footer(QUICKACCESS_HEADER, width));
    std::string empty_line(width, SPACE);
    contents_to_display.emplace_back(empty_line);
    std::size_t usable_lines = height - QUICKACCESS_RESERVED_LINES; // CONSTANT 
    std::size_t max_iter = std::min(paths_list.size(), usable_lines);
    std::ostringstream oss;
    for (std::size_t i = 0; i < max_iter; ++i) {
        std::string path = paths_list[i];
        oss.str(""); 
        oss.clear();
        std::string processed_path = path_length_constraints(path, width - PADDING);
        oss << MAGENTA_TEXT_COLOR << BOLD_ON << (i == pos ? PALE_GREEN_HIGHLIGHT : "");
        oss << SPACE << STAR << SPACE << processed_path << SPACE;
        oss << RESET;
        contents_to_display.emplace_back(oss.str());
    }
    std::size_t remaning_lines = usable_lines > max_iter ?  usable_lines - max_iter: 0;
    while (remaning_lines > 0) {
        contents_to_display.emplace_back(empty_line);
        --remaning_lines;
    }
    contents_to_display.emplace_back(empty_line);
    contents_to_display.emplace_back(get_header_or_footer(QUICKACCESS_FOOTER1, width));
    contents_to_display.emplace_back(get_header_or_footer(QUICKACCESS_FOOTER2, width));
    UI::render_window(contents_to_display);
} 

