#include "Controller.h"
#include "UI.h"
#include <algorithm>
#include <iostream>
#include <stdexcept> // catch and finish the program  

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
        m_quick_access.m_data.emplace_back(std::move(p));
    }
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

bool Controller::handle_navegation(char key) {
    bool stop = false;
    try {
        switch (key) {
        case 'q':
            stop = true;
            break;
        case 'h': // left
            m_explorer.move_backward();
            break;
        case 'l': // right
            m_explorer.move_foward();
            break;
        case 'j': // down
            m_explorer.move_down();
            break;
        case 'k': // up
            m_explorer.move_up();
            break;
        case 'c':
            m_state = STATE::QUICKACCESS;
            break;
        case 's': // select
            m_output_path = m_explorer.get_valid_selected_dir();
            update_quick_list();
            stop = true;
            break;
        default:
            break;
        }
    } catch (const std::runtime_error& e) {
        stop = true;
    }
    return stop;
}

bool Controller::handle_quickaccess(char key) {
    bool stop = false;  
    switch(key) {
        case 'q': // quit
            stop = true;
            break;
        case 'c': // change
            m_state = STATE::NAVEGATION;
            break;
        case 'j': // down
            if (!m_quick_access.m_data.empty()) {
                m_list_pos = m_list_pos == m_total_quick_access_paths - 1 ? 0 : m_list_pos + 1;
            }
            break;
        case 'k': // up
            if (!m_quick_access.m_data.empty()) {
                m_list_pos = m_list_pos == 0 ? m_total_quick_access_paths - 1 : m_list_pos - 1;
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

void Controller::render_window() {
    if (m_state == STATE::NAVEGATION) {
        NavegationView::render_window(
            m_explorer.get_parent_buffer(),
            m_explorer.get_current_buffer(),
            m_explorer.get_child_buffer(),
            m_explorer.get_parent_dir_pos(),
            m_explorer.get_curr_pos(),
            m_explorer.get_curr_displayed_dir()
        );
    }
    else if (m_state == STATE::QUICKACCESS){ 
        QuickAccessView::render_window(m_quick_access.m_data, m_list_pos);
    }
}

void Controller::on_window_resize() {
    render_window();
}

bool Controller::init() {
    // for Windows executes a separate thread where the change of the size is constantly checked
    // std::bind(&Controller::on_window_resize, this) is a way to wrap a member function (on_window_resize) of this particular object (this) into a callable function that can be stored and called later.
    UI::set_window_resize_handler(std::bind(&Controller::on_window_resize, this));
    return UI::init() && m_quick_access.get_state();
}

Controller::Controller(): m_explorer(fs::current_path()) {
    m_state = STATE::QUICKACCESS;
    m_output_path = m_explorer.get_curr_displayed_dir();
    m_list_pos = 0;
    m_total_quick_access_paths = std::min(MAX_QUICK_ACCESS_ITEMS, m_quick_access.m_data.size());
}

void Controller::run()  {
    bool success = init();
    if (!success) {
        return;
    }
#ifdef _WIN32
    std::cerr << CLEAR_WINDOWS_COMMAND_LINE << std::flush();
    // or try UI::clear_command();
#endif
    program_loop();
    finish();
}
