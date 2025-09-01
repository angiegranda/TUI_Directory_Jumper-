#include "Controller.h"
#include "UI.h"
#include <algorithm>
#include <iostream>
#include <stdexcept> // catch and finish the program  

/**
 * @brief Update the Quick Access data when a path is selected.
 * @details
 * This function updates the `QuickAccess` data after a path has been selected:
 * - If the path already exists in the `PathInfo` vector, the current visit count
 *   is appended to its `visits` list.
 * - If the path is new, a new `PathInfo` object is created with the current visit
 *   count as the first entry in its `visits` vector, and then added to the vector.
 * @see QuickAccess
 * @see PathInfo
 */
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

/**
 * @brief Redirects the key (char) pressed to the current state logic until the user selects a path or quit the program.
 * @details
 * - Gets a char input from `UI readchar` 
 * - Depending on the current state the key is redirected to `handle_navegation` or `handle_quickaccess`
 * - Previous functions return a bool `stop`, if true then the loop stops. 
 * - If `stop` is false, it renders the current state window into the terminal.
 * @see UI::readchar()
 * @see UI::render_window()
 */
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

/**
 * @brief Based on the key it calls a function from Explorer, changes the controller state or quits the program.
 * @return bool true if user quicked selected (s) or quit (q)
 */
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

/**
 * @brief Based on the key it manages actions up-down-clean, changes the controller state or quits the program.
 * @return bool true if user quicked selected (s) or quit (q)
 */
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

/**
 * @brief Deactivates `UI::deactivate_window_resize_handler`, saves the quick access data and calls `UI::finish` to close the output stream.
 * @see UI::deactivate_window_resize_handler
 * @see UI::finish
 */
void Controller::finish() {
    UI::deactivate_window_resize_handler();
    m_quick_access.save_data();
    UI::finish();
}

/**
 * @brief Depending on the state it choses the appropiate `render_window` function to display the output.
 * @see NavigationView::render_window
 * @see QuickAccessView::render_window 
 */
void Controller::render_window() {
    if (m_state == STATE::NAVEGATION) {
        NavigationView::render_window(
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


// void Controller::on_window_resize() {
//     render_window();
// }

/**
 * @brief Initialization of `UI::set_window_resize_handler`, `UI::init` and quick access reading and processing of the data file.
 * @see UI::set_window_resize_handler
 * @see UI::init
 */
bool Controller::init() {
    // for Windows executes a separate thread where the change of the size is constantly checked
    // std::bind(&Controller::on_window_resize, this) is a way to wrap a member function (on_window_resize) of this particular object (this) into a callable function that can be stored and called later.
    UI::set_window_resize_handler(std::bind(&Controller::render_window, this));
    return UI::init() && m_quick_access.get_state();
}

/**
 * @brief Constructor, initialize members necessary for quick access state and saves the initial path to be returned in case of quitting the program.
 */
Controller::Controller(): m_explorer(fs::current_path()) {
    m_state = STATE::QUICKACCESS;
    m_output_path = m_explorer.get_curr_displayed_dir();
    m_list_pos = 0;
    m_total_quick_access_paths = std::min(MAX_QUICK_ACCESS_ITEMS, m_quick_access.m_data.size());
}

/**
 * @brief Initialize, run and close the program.
 */
void Controller::run()  {
    bool success = init();
    if (!success) {
        return;
    }
#ifdef _WIN32
    std::cerr << CLEAR_WINDOWS_COMMAND_LINE << std::flush;
    // or try UI::clear_command();
#endif
    program_loop();
    finish();
}
