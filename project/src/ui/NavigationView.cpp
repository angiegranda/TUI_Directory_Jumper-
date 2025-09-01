#include "NavigationView.h"
#include <algorithm>
#include <cstring>
#include <sstream> // ostringstream 

/**
 * @brief Static terminal width parameter changed every time @ref render_window is called. 
 */
std::size_t NavigationView::width;
/**
 * @brief Static terminal height parameter changed every time @ref render_window is called. 
 */
std::size_t NavigationView::height;

/**
 * @brief Renders the navigation window with parent, current, and child buffers.
 * @details
 * - Gets the terminal size.
 * - Chooses a display layout depending on which buffers are available:
 *   - Empty middle buffer → current path is empty, shows an error message. 
 *   - No left buffer → current path is root directory and parent is nullptr, show two windows for current and child buffers.
 *   - No right buffer → child buffer is nullptr because the current visualized document is a file. Show parent + current + file preview.
 *   - Otherwise → show three directory windows (parent, current, child).
 * @param left_contents Parent buffer, may be null if at root.
 * @param middle_contents Current buffer, cannot be null but it can be empty.
 * @param right_contents Child buffer, may be null if current selection is a file.
 * @param parent_pos Current selection index in the parent buffer.
 * @param current_pos Current selection index in the current buffer.
 * @param current_path Path to the current directory.
 */
void NavigationView::render_window(
    const std::shared_ptr<Buffer> left_contents,
    const std::shared_ptr<Buffer> middle_contents,
    const std::shared_ptr<Buffer> right_contents,
    const std::size_t parent_pos,
    const std::size_t current_pos,
    const fs::path& current_path) 
    {
    auto [width_, height_] = UI::get_terminal_size(); // implicit conversion from int to size_t
    NavigationView::width  = std::max<std::size_t>(MIN_TERMINAL_WIDTH, width_);
    NavigationView::height = std::max<std::size_t>(MIN_TERMINAL_HEIGHT, height_);

    if ((middle_contents->get_directories().size() + 
        middle_contents->get_files().size()) == 0) { 
        // The app was launched from an empty directory, error message in the middle so the user moves back
        NavigationView::prepare_display_three_windows(
        left_contents, parent_pos);
    }

    else if (left_contents == nullptr) { 
        // Current directory is root, manages also the case that there exist a file in the rooT
        NavigationView::prepare_two_window_display(
        middle_contents, right_contents, current_pos, current_path);
    }
    else if (right_contents == nullptr) {
        // Display data from the delected file at the current directory 
        NavigationView::prepare_display_three_windows( 
        left_contents, middle_contents, parent_pos, current_pos, current_path);
    }
    else { 
        //Parent, current and child (next) are all directories
        NavigationView::prepare_display_three_windows(
        left_contents, middle_contents, right_contents, parent_pos, current_pos);
    }
}

/**
 * @brief Builds a centered header or footer line.
 * @param text The text to display.
 * @param width_ Width of the line.
 * @return A formatted string padded with spaces and ANSI escape codes.
 */
std::string NavigationView::get_header_or_footer(const char* text, const std::size_t width_) {
    std::size_t remaning_spaces = width_ > std::strlen(text) ? width_ - std::strlen(text) : 0;
    std::size_t corner_spaces = remaning_spaces / 2;
    std::size_t remainder = remaning_spaces % 2;
    std::string line;
    line.reserve(width_);
    line.append(WHITE_TEXT_COLOR);
    line.append(BOLD_ON);
    line.append(std::string(corner_spaces, SPACE));
    line.append(text); 
    line.append(std::string(corner_spaces + remainder, SPACE));
    line.append(RESET);  
    return line;
}

/**
 * @brief Computes layout metrics for rendering.
 * @param column_number Number of columns to display, either 2 or 3.
 * @return A tuple of (usable_lines, base_width, remainder).
 */
Metrics_Tuple NavigationView::get_metrics(const std::size_t column_number) {
    std::size_t total_usable_lines = std::max<std::size_t>(NAVIGATION_MIN_LINES - NAVIGATION_RESERVED_LINES, NavigationView::height - NAVIGATION_RESERVED_LINES);
    std::size_t total_pad = column_number * BETWEEN_COLUMNS_SPACE; //

    std::size_t usable_width = std::max<std::size_t>(MIN_TERMINAL_WIDTH - total_pad, NavigationView::width - total_pad);
    std::size_t base_width = usable_width / column_number;
    std::size_t remainder  = usable_width % column_number;

    return std::make_tuple(total_usable_lines, base_width, remainder);
}

/**
 * @brief Cleans and formats file contents for display.
 * @details
 * - Replaces common non-printable characters with spaces.
 * - Truncates or pads lines to fit column width.
 * - If too many non-printables are detected given some threshold `BINARY_HEURISTIC`, marks file as binary.
 * - Pads output with empty lines to match total display lines.
 * @param contents Lines of file content to process.
 * @param width_ Column width.
 * @param total_lines Number of lines to fill.
 */
void NavigationView::process_text(std::vector<std::string>& contents, 
    const std::size_t width_, const std::size_t total_lines) {

    std::string empty_line(width_, SPACE);
    std::size_t total_chars = 0;
    std::size_t non_printable_chars = 0;
    std::size_t curr_line_pos = 0;

    for (std::size_t i = 0; i < contents.size(); ++i) {
        std::string clean;
        clean.reserve(contents[i].size());  
        for (unsigned char char_ : contents[i]) {
            ++total_chars;
            if (char_ == TAB || char_ == NEW_LINE || char_ == CARRIAGE_RETURN) char_ = SPACE;
            if (char_ >= MIN_ASCII && char_ <= MAX_ASCII) {
                clean += char_;
            } else {
                ++non_printable_chars;
            }
        }
        ++curr_line_pos;
        contents[i] = (clean.size() >= width_) ? 
        clean.substr(0, width_) : (clean + std::string(width_ - clean.size(), SPACE));
    }

    if (static_cast<double>(non_printable_chars) / total_chars > BINARY_HEURISTIC) { 
        contents.clear();
        if (std::strlen(BINARY_FILE) >= width_) {
            contents.emplace_back(std::string(BINARY_FILE).substr(0, width_));
        } else {
            contents.emplace_back(BINARY_FILE + std::string(width_ - std::strlen(BINARY_FILE) , SPACE));
        }
        curr_line_pos = 1;
    }
    while (curr_line_pos < total_lines) {
        contents.emplace_back(empty_line);
        ++curr_line_pos;
    }
}

/**
 * @brief Reads file contents and prepares them for display.
 * @details
 * - Opens file at given path, reads up to total_lines.
 * - Cleans lines via @ref process_text.
 * - If file cannot be opened, shows error message instead.
 * @param file_path Path of the file to read.
 * @param width_ Column width.
 * @param total_lines Number of lines to display.
 * @return Vector of processed strings ready for rendering.
 */
std::vector<std::string> NavigationView::get_text_contents(
    fs::path file_path, const std::size_t width_, const std::size_t total_lines) {
    std::vector<std::string> contents;
    std::ifstream file(file_path);

    if (!file.is_open()) {
        std::string info = ERROR_OPEN_FILE + file_path.string();
        contents.emplace_back(std::move(info));
        process_text(contents, width_, total_lines);
        return contents;
    }

    std::string line; 
    std::size_t pos = 0;
    while (std::getline(file, line) && pos < total_lines) {
        if (line.empty()) { continue; }
        contents.emplace_back(std::move(line)); // is it safe to do std::move here?
        ++pos;
    }

    if (pos == 0) {
        contents.emplace_back(EMPTY_FILE);
    }
    process_text(contents, width_, total_lines);
    file.close();
    return contents;
}

/**
 * @brief Prepares a buffer’s directories and files for display in a column.
 * @details
 * - Renders directories first, then files.
 * - Highlights the selected entry if called for parent-current buffer, for child is ignored.
 * - Truncates names that exceed column width, appending "…".
 * - Pads the result to total_lines.
 * @param buffer Buffer to render (directories + files).
 * @param pos Index of the currently selected entry.
 * @param has_selected_dir Whether the highlighted entry should be marked.
 * @param col_width Width of the column.
 * @param total_lines Number of lines to display.
 * @return Vector of formatted strings for this column.
 */
std::vector<std::string> NavigationView::preprocess_contents(
    const std::shared_ptr<Buffer> buffer, 
    const std::size_t pos, 
    bool has_selected_dir,
    const std::size_t col_width, 
    const std::size_t total_lines) {

    std::vector<std::string> processed_output;

    const auto& dirs  = buffer->get_directories();
    const auto& files = buffer->get_files();
    std::size_t M = dirs.size();
    std::size_t N = files.size();

    std::size_t start = (pos >= total_lines) ? (pos - total_lines + 1) : 0;
    std::size_t end   = (pos >= total_lines) ? pos : (total_lines - 1);
    end = std::min(end, M + N - 1); 

    std::string empty_line(col_width, SPACE);
    std::size_t curr_line = 0;
    std::ostringstream oss; 

    for (std::size_t idx = start; idx <= end && curr_line < total_lines; ++idx, ++curr_line) {
        oss.str("");
        oss.clear(); 
        bool highlighted = has_selected_dir && (idx == pos);
        if (idx < M) {
            const auto& entry = dirs[idx];
            oss << MAGENTA_TEXT_COLOR << BOLD_ON << (highlighted? PALE_GREEN_HIGHLIGHT : "");
            oss << ((entry.size() > col_width && col_width > std::strlen(DOTS)) 
                ? entry.substr(0, col_width - std::strlen(DOTS)) + DOTS
                : entry + std::string(col_width > entry.size() ? col_width - entry.size() : 0, SPACE));
        } else if (idx < M + N) {
            if (highlighted) {
                oss << BLACK_TEXT_COLOR << PALE_GREEN_HIGHLIGHT;
            } else {
                oss << WHITE_TEXT_COLOR;
            }
            const auto& entry = files[idx - M];
            oss << ((entry.size() > col_width && col_width > std::strlen(DOTS)) 
                ? entry.substr(0, col_width - std::strlen(DOTS)) + DOTS
                : entry + std::string(col_width > entry.size() ? col_width - entry.size() : 0, SPACE));
        } else {
            oss << empty_line;
        }
        oss << RESET;
        processed_output.emplace_back(oss.str());
    }
    while (curr_line < total_lines) {
        processed_output.emplace_back(empty_line);
        ++curr_line;
    }
    return processed_output;
}

/**
 * @brief Adds a header section to the output display.
 * @param contents_to_display Output vector where header lines are appended.
 */
void NavigationView::add_header(std::vector<std::string>& contents_to_display) {
    contents_to_display.emplace_back(get_header_or_footer(NAVIGATION_HEADER, NavigationView::width));
    contents_to_display.emplace_back(std::string(NavigationView::width, SPACE));
}

/**
 * @brief Adds a footer section to the output display.
 * @param contents_to_display Output vector where footer lines are appended.
 */
void NavigationView::add_footer(std::vector<std::string>& contents_to_display) {
    contents_to_display.emplace_back(std::string(NavigationView::width, SPACE));
    contents_to_display.emplace_back(get_header_or_footer(NAVIGATION_FOOTER1, NavigationView::width));
    contents_to_display.emplace_back(get_header_or_footer(NAVIGATION_FOOTER2, NavigationView::width));
}

/**
 * @brief Prepares display contents for a two-column layout.
 * @param left_contents Left column (current buffer directories/files preprocessed).
 * @param right_contents Right column (child buffer directories/files preprocessed or file preview).
 * @param total_lines Number of lines to display.
 * @return Vector of combined display lines.
 */
std::vector<std::string> NavigationView::prepare_contents(
    const std::vector<std::string>& left_contents, 
    const std::vector<std::string>& right_contents, 
    const std::size_t total_lines) {
    std::vector<std::string> contents_to_display;
    add_header(contents_to_display);
    for (std::size_t i = 0; i < total_lines; ++i) {
        contents_to_display.emplace_back(
            SPACE + 
            left_contents[i] + 
            std::string(BETWEEN_COLUMNS_SPACE, SPACE) + 
            right_contents[i] + 
            SPACE
        ); 
    }
    add_footer(contents_to_display);
    return contents_to_display;
}

/**
 * @brief Prepares display contents for a three-column layout.
 * @param left_contents Left column (parent buffer directories/files preprocessed).
 * @param middle_contents Middle column (current buffer directories/files preprocessed).
 * @param right_contents Right column (child buffer preprocessed or file preview).
 * @param total_lines Number of lines to display.
 * @return Vector of combined display lines.
 */
std::vector<std::string> NavigationView::prepare_contents(
    const std::vector<std::string>& left_contents, 
    const std::vector<std::string>& middle_contents,
    const std::vector<std::string>& right_contents, 
    const std::size_t total_lines) {

    std::vector<std::string> contents_to_display;
    add_header(contents_to_display);
    for (std::size_t i = 0; i < total_lines; ++i) {
        contents_to_display.emplace_back(
            SPACE + 
            left_contents[i] + 
            std::string(BETWEEN_COLUMNS_SPACE, SPACE) + 
            middle_contents[i] + 
            std::string(BETWEEN_COLUMNS_SPACE, SPACE) + 
            right_contents[i] + 
            SPACE
        );
    }
    add_footer(contents_to_display);
    return contents_to_display;
}

/**
 * @brief Renders a two-window display.
 * @details
 * - Used when at current buffer points at the contents of the root. 
 * - Right window shows either child directories or file preview.
 * @param left_buffer Left buffer (current directory contents).
 * @param right_buffer Right buffer, nullptr if the selected entry with index `current_pos` is a file.
 * @param current_pos Index of the selected entry in left buffer.
 * @param current_path Path of the current directory.
 */
void NavigationView::prepare_two_window_display(
    const std::shared_ptr<Buffer> left_buffer,
    const std::shared_ptr<Buffer> right_buffer, 
    const std::size_t current_pos, 
    const fs::path& current_path) {
    bool contains_text = false;
    if (right_buffer == nullptr) {  
        contains_text = true;
    }
    auto [total_lines, col_width, reminder] = get_metrics(2);
    auto left_contents = preprocess_contents(left_buffer, current_pos, true, col_width, total_lines);
    std::vector<std::string> right_contents;
    if (!contains_text) {
        right_contents = preprocess_contents(right_buffer, 0, false, col_width+reminder, total_lines);
    } else {
        if (left_buffer->get_files().size() == 0) {
            std::string empty_line(col_width, SPACE);
            for (std::size_t i = 0; i < total_lines; ++i) {
                right_contents.emplace_back(empty_line);
            }
        } else {
            std::string filename = left_buffer->get_files()[current_pos - left_buffer->get_directories().size()];
            fs::path file_path = current_path / filename;
            right_contents = get_text_contents(file_path, col_width + reminder, total_lines);
        }
    }
    UI::render_window(prepare_contents(left_contents, right_contents, total_lines));
}

/**
 * @brief Renders three windows when the middle buffer is empty.
 * @details
 * - Shows parent contents on the left.
 * - Displays an error message in the middle column.
 * - Shows an empty right column.
 * @param left_buffer Parent buffer.
 * @param parent_pos Index of selected entry in parent buffer.
 */
void NavigationView::prepare_display_three_windows(
    const std::shared_ptr<Buffer> left_buffer,
    const std::size_t parent_pos) {
    
    auto get_error_message = [](const char* name, std::size_t width_) -> std::string {
        std::size_t pad_spaces = (width_ - std::strlen(name)) / 2;
        std::size_t reminder = (width_ - std::strlen(name)) % 2;
        std::ostringstream oss;
        oss << BOLD_ON << BLACK_TEXT_COLOR << PALE_GREEN_HIGHLIGHT;
        oss << std::string(pad_spaces, SPACE) << name << std::string(pad_spaces + reminder, SPACE);
        oss << RESET;
        return oss.str();
    };

    auto [total_lines, col_width, reminder] = get_metrics(3);
    auto left_contents = preprocess_contents(left_buffer, parent_pos, true, col_width, total_lines); // EMPTY CHECK CORRECTNESS
    std::string empty_line(col_width, ' ');
    std::vector<std::string> middle_contents; 
    std::vector<std::string> right_contents;
    middle_contents.emplace_back(get_error_message(EMPTY_DIR_MESSAGE1, col_width));
    middle_contents.emplace_back(get_error_message(EMPTY_DIR_MESSAGE2, col_width));
    for (std::size_t i = 0; i < total_lines; ++i) {
        if (i < total_lines - 2) {
            middle_contents.emplace_back(empty_line);
        }
        right_contents.emplace_back(empty_line);
    }
    UI::render_window(prepare_contents(left_contents, middle_contents, right_contents, total_lines));
}

/**
 * @brief Renders three windows when displaying a file.
 * @details
 * - This function is selected when the child buffer is a nullptr.
 * - The path to the selected entry file is computed and with @ref get_text_contents its contents are prepared for being displayed.
 * @param left_buffer Parent buffer.
 * @param middle_buffer Current buffer.
 * @param parent_pos Index of selected entry in parent buffer.
 * @param current_pos Index of selected file in current buffer.
 * @param current_path Path of the current directory.
 */
void NavigationView::prepare_display_three_windows(
    const std::shared_ptr<Buffer> left_buffer, 
    const std::shared_ptr<Buffer> middle_buffer, 
    const std::size_t parent_pos, 
    const std::size_t current_pos, 
    const fs::path& current_path) {

    auto [total_lines, col_width, reminder] = get_metrics(3);

    std::string filename = middle_buffer->get_files()[current_pos - middle_buffer->get_directories().size()];
    fs::path file_path = current_path / filename;
    auto text = get_text_contents(file_path, col_width + reminder, total_lines);

    auto left_contents = preprocess_contents(left_buffer, parent_pos, true, col_width, total_lines);
    auto middle_contents = preprocess_contents(middle_buffer, current_pos, true, col_width, total_lines);

    UI::render_window(prepare_contents(left_contents, middle_contents, text, total_lines));
}

/**
 * @brief Renders three windows with parent, current, and child buffers.
 * @details
 * - Used when all three buffers are available.
 * @param left_buffer Parent buffer.
 * @param middle_buffer Current buffer.
 * @param right_buffer Child buffer.
 * @param parent_pos Index of selection in parent buffer.
 * @param current_pos Index of selection in current buffer.
 */
void NavigationView::prepare_display_three_windows(
    const std::shared_ptr<Buffer> left_buffer, 
    const std::shared_ptr<Buffer> middle_buffer, 
    const std::shared_ptr<Buffer> right_buffer, 
    const std::size_t parent_pos, 
    const std::size_t current_pos) {

    auto [total_lines, col_width, reminder] = get_metrics(3);

    auto left_contents = preprocess_contents(left_buffer, parent_pos, true, col_width, total_lines);
    auto middle_contents = preprocess_contents(middle_buffer, current_pos, true, col_width, total_lines);
    auto right_contents = preprocess_contents(right_buffer, 0, false, col_width+reminder, total_lines);
    
    UI::render_window(prepare_contents(left_contents, middle_contents, right_contents, total_lines));
}
