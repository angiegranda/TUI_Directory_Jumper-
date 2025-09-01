#include "QuickAccessView.h"
#include "QuickAccess.h"
#include <algorithm> // std::min
#include <sstream> // ostringstream 
#include <stack> 
#include <cstring> // strlen

/**
 * @brief Formats a filesystem path to fit within a given width.
 * @details
 * - If the path length is shorter than the width, it is padded with spaces.
 * - If the path is longer, leading directories are truncated and replaced with
 *   a shortcut marker `DIR_SHORTCUT`, keeping only the trailing components.
 * - Always returns a string exactly `width` characters long.
 * @param path Filesystem path.
 * @param width Maximum width of the rendered string.
 * @return Formatted path string with truncation and padding applied.
 * @see Constants.h
 */
std::string QuickAccessView::path_length_constraints(const std::string& path, const std::size_t width) {
    std::string line;
    line.reserve(width);
    if (path.size() < width) {
        line.append(path);
        return line.append(std::string(width - path.size(), SPACE));
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
    line.append(DIR_SHORTCUT);
    while (!stack_.empty()) {
        line.append(SLASH);
        line.append(stack_.top());
        stack_.pop();
    }
    line.append(std::string((width < line.size() ? 0 : width - line.size()), SPACE));
    return line;
}

/**
 * @brief Builds a centered header or footer line for the quick access view.
 * @param text Text to display in the header/footer.
 * @param width Total width of the line.
 * @return A formatted string with padding and ANSI escape codes.
 * @see Constants.h
 */
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

/**
 * @brief Renders the quick access window.
 * @details
 * - Gets the current terminal size.
 * - Displays a header, followed by a list of paths, then a footer.
 * - Each path is formatted with @ref path_length_constraints to fit the width.
 * - Highlights the entry at the given cursor position `pos`.
 * - Pads with empty lines to fill the terminal height.
 * @param paths List of paths with metadata PathInfo.
 * @param pos Index of the currently selected path.
 * @see PathInfo
 */
void QuickAccessView::render_window(const std::vector<PathInfo>& paths, std::size_t pos)  {
    std::vector<std::string> contents_to_display;
    auto [width_, height_] = UI::get_terminal_size();
    std::size_t width  = std::max<std::size_t>(MIN_TERMINAL_WIDTH, width_);
    std::size_t height = std::max<std::size_t>(MIN_TERMINAL_HEIGHT, height_);

    contents_to_display.emplace_back(get_header_or_footer(QUICKACCESS_HEADER, width));
    std::string empty_line(width, SPACE);
    contents_to_display.emplace_back(empty_line);
    std::size_t usable_lines = height - QUICKACCESS_RESERVED_LINES; // CONSTANT 
    std::size_t max_iter = std::min(paths.size(), usable_lines);
    std::ostringstream oss; 
    for (std::size_t i = 0; i < max_iter; ++i) {
        oss.str("");
        oss.clear();    
        oss << MAGENTA_TEXT_COLOR << BOLD_ON << (i == pos ? PALE_GREEN_HIGHLIGHT : "");
        oss << SPACE << STAR << SPACE << path_length_constraints(paths[i].m_path, width - PADDING);
        oss << SPACE << RESET;
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

