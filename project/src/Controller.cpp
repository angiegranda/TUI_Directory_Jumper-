#include "Controller.h"
#include "UI.h"
#include <iostream>

// ------------------------------ NAVEGATION ACTIONS --------------------------------


void Controller::update_dir_options() {
    auto buf = m_explorer.get_current_buffer();
    m_curr_options = buf ? buf->get_directories().size() + buf->get_files().size() : 0;
}

void Controller::navegation_move_forward() {
    if (m_curr_doc == DOC_TYPE::FILE || 
        (m_explorer.get_child_buffer()->get_directories().size() + 
        m_explorer.get_child_buffer()->get_files().size() == 0))
        return;
    m_explorer.move_foward();
    update_dir_options();
    m_curr_doc = m_explorer.get_child_buffer() == nullptr ? DOC_TYPE::FILE : DOC_TYPE::DIRECTORY; 
    m_curr_pos = 0;
}

void Controller::navegation_move_backward() {
    m_curr_pos = m_explorer.get_parent_dir_pos();
    m_explorer.move_backward();
    update_dir_options();
    m_curr_doc = m_explorer.get_child_buffer() == nullptr ? DOC_TYPE::FILE : DOC_TYPE::DIRECTORY; 
}

void Controller::navegation_move_down() {
    if (m_curr_options > 0) { // both FILEs and documents 
        m_curr_pos = (m_curr_pos + 1) % m_curr_options;
        std::size_t dirs_size = m_explorer.get_current_buffer()->get_directories().size();
        m_curr_doc = m_curr_pos < dirs_size ? DOC_TYPE::DIRECTORY : DOC_TYPE::FILE;
        if (m_curr_doc == DOC_TYPE::DIRECTORY) {
            std::string curr_selected_dir = m_explorer.get_current_buffer()->get_directories()[m_curr_pos];
            m_explorer.update_child_buffer(curr_selected_dir);
            update_dir_options();
        }
    }
}

void Controller::navegation_move_up() {
    if (m_curr_options > 0) {
        m_curr_pos = (m_curr_pos == 0) ? m_curr_options - 1 : m_curr_pos - 1;
        std::size_t dirs_size = m_explorer.get_current_buffer()->get_directories().size();
        m_curr_doc = m_curr_pos < dirs_size ? DOC_TYPE::DIRECTORY : DOC_TYPE::FILE;
        if (m_curr_doc == DOC_TYPE::DIRECTORY) {
            std::string curr_selected_dir = m_explorer.get_current_buffer()->get_directories()[m_curr_pos];
            m_explorer.update_child_buffer(curr_selected_dir);
            update_dir_options();
        }
    }
   //std::cout << m_curr_options << " " << m_curr_pos << std::endl;
}


// ------------------------------ QUICKACCESS ACTIONS --------------------------------



void Controller::update_quick_list() {
    ++m_quick_access.m_total_visits;
    if (m_state == STATE::QUICKACCESS) {
        m_quick_access.m_data[m_list_pos].m_visits.emplace_back(m_quick_access.m_total_visits);
        return;
    }
    // selected a directory from navegation so we need to find it or create a new path info
    bool found = false; 
    for (auto&& entry : m_quick_access.m_data) {
        if (entry.m_path == m_output_path.string()) {
            found = true;
            entry.m_visits.emplace_back(m_quick_access.m_total_visits);
            break;
        }
    }
    if (!found) {
        PathInfo p;
        p.m_path = m_output_path.string();
        p.m_visits.emplace_back(m_quick_access.m_total_visits);
        m_quick_access.m_data.emplace_back(p);
    }
}

std::vector<std::string> Controller::quick_access_paths() {
    std::vector<std::string> paths;
    std::size_t max_num = std::min(MAX_QUICK_ACCESS_ITEMS, m_quick_access.m_data.size());
    for (std::size_t idx = 0; idx < max_num; ++idx) {
        paths.emplace_back(m_quick_access.m_data[idx].m_path);
    }
    return paths;
}



// --------------------------- PROGRAM WORKFLOW -----------------------------


Controller::Controller(): m_explorer(fs::current_path()) {
    m_state = STATE::QUICKACCESS;
    m_output_path = m_explorer.get_current_path();
    m_curr_doc = m_explorer.get_child_buffer() == nullptr ? DOC_TYPE::FILE : DOC_TYPE::DIRECTORY;
    m_curr_pos = 0;
    m_list_pos = 0;
}

bool Controller::init() {
    // for Windows executes a separate thread where the change of the size is constantly checked
    // std::bind(&Controller::on_window_resize, this) is a way to wrap a member function (on_window_resize) of this particular object (this) into a callable function that can be stored and called later.
    UI::set_window_resize_handler(std::bind(&Controller::on_window_resize, this));
    return UI::init();
}

void Controller::program_loop() {
    char key;
    bool stop = false;
    while (!stop) {
        render_window();
        key = UI::readchar();
        if (m_state == STATE::NAVEGATION) {
            stop = handle_navegation(key);
        }
        else if (m_state == STATE::QUICKACCESS) {
            stop = handle_quickaccess(key);
        }
    }
    std::cout << m_output_path.string() << std::endl;
}

// true if continue else false;
bool Controller::handle_navegation(char key) {
    bool stop = false;
    update_dir_options(); 
    switch (key) {
        case 'q':
            stop = true;
            break;
        case 'h': // left
            navegation_move_backward();
            break;
        case 'l': // right
            navegation_move_forward();
            break;
        case 'j': // down
            navegation_move_down();
            break;
        case 'k': // up
            navegation_move_up();
            break;
        case 'c':
            m_state = STATE::QUICKACCESS;
            break;
        case 's': // select
            if (m_curr_doc == DOC_TYPE::FILE) {
                m_output_path = m_explorer.get_current_path();
            } else {
                m_output_path = m_explorer.get_path_to_visit();
            }
            update_quick_list();
            stop = true;
            break;
        default:
            break;
    }
    return stop;
}

bool Controller::handle_quickaccess(char key) {
    bool stop = false;  
    std::size_t max_num = std::min(MAX_QUICK_ACCESS_ITEMS, m_quick_access.m_data.size());
    switch(key) {
        case 'q': // quit
            stop = true;
            break;
        case 'c': // change
            m_state = STATE::NAVEGATION;
            break;
        case 'j': // down
            if (!m_quick_access.m_data.empty()) {
                m_list_pos = m_list_pos == max_num - 1 ? 0 : m_list_pos + 1;
            }
            break;
        case 'k': // up
            if (!m_quick_access.m_data.empty()) {
                m_list_pos = m_list_pos == 0 ? max_num - 1 : m_list_pos - 1;
            }
            break;
        case 's': // select from the directories shown
            if (!m_quick_access.m_data.empty()) {
               m_output_path = fs::path(m_quick_access.m_data[m_list_pos].m_path);
               update_quick_list();
               stop = true;
            }
            break;
        case 'x': // clean history
            m_quick_access.m_data.clear();
            m_quick_access.m_total_visits = 0;
            break;
        default:
            break;
    }
    return stop;
}

void Controller::finish() {
    UI::deactivate_window_resize_handler();
    m_quick_access.save_data();
    UI::finish();
}

void Controller::run()  {
    bool success = init();
    if (!success) {
        return;
    }
    program_loop();
    finish();
}

void Controller::render_window() {
    if (m_state == STATE::NAVEGATION) {
        n_view.render_window(
            m_explorer.get_parent_buffer(),
            m_explorer.get_current_buffer(),
            m_explorer.get_child_buffer(),
            m_explorer.get_parent_dir_pos(),
            m_curr_pos,
            m_explorer.get_current_path());
    }
    else if (m_state == STATE::QUICKACCESS){ 
        q_view.render_window(quick_access_paths(), m_list_pos);
    }
}

void Controller::on_window_resize() {
    render_window();
}
