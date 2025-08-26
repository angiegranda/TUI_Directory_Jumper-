#ifndef _CFD_NAVEGATION_VIEW_H_
#define _CFD_NAVEGATION_VIEW_H_

#include "TerminalConstants.h"
#include "Constants.h"
#include "UI.h"
#include "Explorer.h"


class NavegationView {

    private:

    std::size_t width;
    std::size_t height;

    void prepare_two_window_display(
        const std::shared_ptr<Buffer> middle_buffers,
        const std::shared_ptr<Buffer> right_buffer, 
        const std::size_t current_pos, 
        const fs::path& current_path);

    // looking at a file in the current path
    void prepare_display_three_windows(
        const std::shared_ptr<Buffer> left_buffer, 
        const std::shared_ptr<Buffer> middle_buffer, 
        const std::size_t parent_pos, 
        const std::size_t current_pos, 
        const fs::path& current_path);

    // looking at a folder in the current path
    void prepare_display_three_windows(
        const std::shared_ptr<Buffer> left_buffer, 
        const std::shared_ptr<Buffer> middle_buffer, 
        const std::shared_ptr<Buffer> right_buffer, 
        const std::size_t parent_pos, 
        const std::size_t current_pos);

    std::string get_header_or_footer(const char* text, const std::size_t width_);

    Metrics_Tuple get_metrics(const std::size_t column_number);

    std::vector<std::string> get_text_contents(fs::path file_path, const std::size_t width_, const std::size_t total_lines);
    void process_text(std::vector<std::string>& contents, const std::size_t width_, const std::size_t total_lines);
    
    std::vector<std::string> preprocess_contents(
    const std::shared_ptr<Buffer> contents, 
    const std::size_t pos, 
    bool has_selected_dir,
    const std::size_t row_width, 
    const std::size_t total_lines);

    void add_header(std::vector<std::string>& contents);
    void add_footer(std::vector<std::string>& contents);

    public: 

    void render_window(
    const std::shared_ptr<Buffer> left_contents,
    const std::shared_ptr<Buffer> middle_contents,
    const std::shared_ptr<Buffer> right_contents,
    const std::size_t parent_pos,
    const std::size_t current_pos,
    const fs::path& current_path);

};

#endif