/**
 * @file Controller.h
 * @author Angie Granda
 * @brief Contains Controller class which manages the logic of the program.
 * @date 2025-08-30
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef _CFD_CONTROLLER_H_
#define _CFD_CONTROLLER_H_
#include "Explorer.h"
#include "QuickAccess.h"
#include "NavigationView.h"
#include "QuickAccessView.h"

enum class STATE { NAVEGATION, QUICKACCESS};

/**
 * @class Controller
 * @brief This class receives inputs (keys) and perfoms the actions offered by the program.
 * @details 
 * - Initializes members and checks that UI and quick access read/write files are accessible. 
 * If not then the program will not start.
 * - Receives the key inputs from the user and processes the chosen action depending on the controller state.
 * - Manages a state which can switch back-and-forth from displaying navigation and quickaccess.
 * - Closes the streams. 
 * - If there was an exception std::bad_alloc then it closes the program immediately.
 * @see QuickAccess
 * @see Explorer
 * @see NavigationView
 * @see QuickAccessView
 * @see UI
 */

class Controller {

private:

    STATE m_state;
    fs::path m_output_path;

    // Navigation members 
    Explorer m_explorer;

    // QuickAccess members;
    QuickAccess m_quick_access;
    std::size_t m_list_pos;
    std::size_t m_total_quick_access_paths;

    void render_window();
    void update_quick_list();

    // PROGRAM
    bool init();
    void program_loop();
    void finish();
    //void on_window_resize();

    bool handle_navegation(char key);
    bool handle_quickaccess(char key);

public:

    Controller();
    void run();
};

#endif 
