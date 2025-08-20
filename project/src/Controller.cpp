#include "Controller.h"
#include "UI.h"
#include <iostream>



Controller::Controller(fs::path current_path): m_explorer(current_path) {}

void Controller::init() {
    // for Windows executes a separate thread where the change of the size is constantly checked
    UI::set_window_resize_handler(std::bind(&Controller::on_window_resize, this));
}


void Controller::program_loop() {
    enableAnsiOnWindows();              // enable ANSI on Windows
    // std::cout << ENTER_ALT_SCREEN;      // alternate screen 
    std::cout << HIDE_CURSOR;   
    char c;
    std::size_t dir_options = m_explorer.get_current_buffer()->get_directories().size();
    std::size_t curr_pos = 0;
    std::string curr_selected_dir;
    UI::render_window(m_explorer.get_parent_data(), m_explorer.get_current_data(), m_explorer.get_child_data());
    for (;;) {
        c = UI::readchar();
        if (c == 'q') {
            std::cout.flush();
            break;
        }
        switch (c) {
            case 'h': // left
                m_explorer.move_backward();
                dir_options = m_explorer.get_current_buffer()->get_directories().size();
                break;
            case 'j': // down
                if (dir_options == 0) {
                    break;
                }
                curr_pos = curr_pos + 1 >= dir_options ? 0 : curr_pos + 1;
                curr_selected_dir = m_explorer.get_current_buffer()->get_directories()[curr_pos];
                m_explorer.update_child_buffer(curr_selected_dir);
                break;
            case 'k': // up
                if (dir_options == 0) {
                    break;
                }
                curr_pos = curr_pos <= 0 ? dir_options - 1 : curr_pos - 1;
                curr_selected_dir = m_explorer.get_current_buffer()->get_directories()[curr_pos];
                m_explorer.update_child_buffer(curr_selected_dir);
                break;
            case 'l': // right
                m_explorer.move_foward();
                dir_options = m_explorer.get_current_buffer()->get_directories().size();
                curr_pos = 0;
                break;
            default:
                break;
        }

        UI::render_window(m_explorer.get_parent_data(), m_explorer.get_current_data(), m_explorer.get_child_data());
    }
    std::cout << SHOW_CURSOR;
    // std::cout << EXIT_ALT_SCREEN;    // exit alternative screen
}

void Controller::finish() {
    UI::deactivate_window_resize_handler();
}

void Controller::run() {
    init();
    program_loop();
    finish();
}

void Controller::on_window_resize() const {
    UI::render_window(m_explorer.get_parent_data(), m_explorer.get_current_data(), m_explorer.get_child_data());
}
