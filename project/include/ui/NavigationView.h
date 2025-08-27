#ifndef _CFD_NAVIGATION_VIEW_H_
#define _CFD_NAVIGATION_VIEW_H_

#include "TerminalConstants.h"
#include "Constants.h"
#include "UI.h"
#include "Explorer.h"


class NavegationView {

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
    static const std::shared_ptr<Buffer> contents, 
    static const std::size_t pos, 
    static bool has_selected_dir,
    static const std::size_t row_width, 
    static const std::size_t total_lines);

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