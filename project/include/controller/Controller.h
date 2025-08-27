#ifndef _CFD_CONTROLLER_H_
#define _CFD_CONTROLLER_H_
#include "Explorer.h"
#include "QuickAccess.h"
#include "NavigationView.h"
#include "QuickAccessView.h"

enum class STATE { NAVEGATION, QUICKACCESS, ARGS_FAILURE};

class Controller {

private:

    STATE m_state;
    fs::path m_output_path;

    // Navigation members 
    Explorer m_explorer;
    NavegationView n_view;

    // QuickAccess members;
    QuickAccess m_quick_access;
    QuickAccessView q_view;
    std::size_t m_list_pos;
    std::size_t m_total_quick_access_paths;

    void render_window();
    void update_quick_list();

    // PROGRAM
    bool init();
    void program_loop();
    void finish();
    void on_window_resize();

    bool handle_navegation(char key);
    bool handle_quickaccess(char key);

public:

    Controller();
    void run();
};

#endif 
