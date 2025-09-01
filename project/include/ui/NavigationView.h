/**
 * @file NavigationView.h
 * @author Angie Granda 
 * @brief Prepares and formats navigation data for terminal display.
 * @date 2025-08-31
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef _CFD_NAVIGATION_VIEW_H_
#define _CFD_NAVIGATION_VIEW_H_

#include "TerminalConstants.h"
#include "Constants.h"
#include "UI.h"
#include "Explorer.h"

/**
 * @class NavigationView
 * @brief Process the contents of buffers and text files for rendering in the UI.
 * @details
 * - Based on the state of the smart shared pointers to buffer it will handle the how many columns to display 
 * and the information in each column.
 * - If the currently selected document is a file, the class will attempt to read it.
 *   Depending on the result, it will display:
 *     - The beginning of the file’s content if readable text is present.
 *     - A message `[Empty File]` if the file is empty.
 *     - A message `[Binary File]` if the file contains mostly non-printable characters.
 *     - An error message if the file could not be opened or read.
 * - Prepares the headers, row contents and footers.
 * - Delegates final rendering to the static `UI` class once formatting is complete.
 * @see Buffer
 * @see UI
 * @see Constants.h
 */

class NavigationView {

    private:

    static std::size_t width;
    static std::size_t height;

    // render window helpers 

    static void prepare_two_window_display(
    const std::shared_ptr<Buffer> left_buffer,
    const std::shared_ptr<Buffer> right_buffer, 
    const std::size_t current_pos, 
    const fs::path& current_path);

    static void prepare_display_three_windows(
    const std::shared_ptr<Buffer> left_buffer,
    const std::size_t parent_pos);

    static void prepare_display_three_windows(
    const std::shared_ptr<Buffer> left_buffer, 
    const std::shared_ptr<Buffer> middle_buffer, 
    const std::size_t parent_pos, 
    const std::size_t current_pos, 
    const fs::path& current_path);

    static void prepare_display_three_windows(
    const std::shared_ptr<Buffer> left_buffer, 
    const std::shared_ptr<Buffer> middle_buffer, 
    const std::shared_ptr<Buffer> right_buffer, 
    const std::size_t parent_pos, 
    const std::size_t current_pos);
    
    static std::vector<std::string> prepare_contents(
    const std::vector<std::string>& right_contents, 
    const std::vector<std::string>& left_contents, 
    const std::size_t total_lines); 

    static std::vector<std::string> prepare_contents(
    const std::vector<std::string>& right_contents, 
    const std::vector<std::string>& middle_contents,
    const std::vector<std::string>& left_contents, 
    const std::size_t total_lines); 

    // preprocessing  

    static std::string get_header_or_footer(const char* text, const std::size_t width_);
    static Metrics_Tuple get_metrics(const std::size_t column_number);
    static void add_header(std::vector<std::string>& contents);
    static void add_footer(std::vector<std::string>& contents);

    static std::vector<std::string> get_text_contents(fs::path file_path, const std::size_t width_, const std::size_t total_lines);
    static void process_text(std::vector<std::string>& contents, const std::size_t width_, const std::size_t total_lines);
    
    static std::vector<std::string> preprocess_contents(
    const std::shared_ptr<Buffer> contents, 
    const std::size_t pos, 
    bool has_selected_dir,
    const std::size_t row_width, 
    const std::size_t total_lines);

    public: 

    static void render_window(
    const std::shared_ptr<Buffer> left_contents,
    const std::shared_ptr<Buffer> middle_contents,
    const std::shared_ptr<Buffer> right_contents,
    const std::size_t parent_pos,
    const std::size_t current_pos,
    const fs::path& current_path);

};

#endif