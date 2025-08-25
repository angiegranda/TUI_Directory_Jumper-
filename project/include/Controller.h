#ifndef _CFD_CONTROLLER_H_
#define _CFD_CONTROLLER_H_
#include "Explorer.h"
#include "QuickAccess.h"
#include "NavegationView.h"
#include "QuickAccessView.h"

enum class DOC_TYPE { DIRECTORY, FILE };
enum class STATE { NAVEGATION, QUICKACCESS, ARGS_FAILURE};

class Controller {

private:

    DOC_TYPE m_curr_doc;
    STATE m_state;

    // Navegation members 
    Explorer m_explorer;
    fs::path m_output_path;
    std::size_t m_curr_options;
    std::size_t m_curr_pos;
    NavegationView n_view;

    // QuickAccess members;
    QuickAccess m_quick_access;
    std::size_t m_list_pos;
    QuickAccessView q_view;

    void parse_args(int argc, char* argv[]);
    void render_window();

    // NAVEGATION
    void update_dir_options();
    void navegation_move_forward(); 
    void navegation_move_backward();
    void navegation_move_down();
    void navegation_move_up();

    // QUICKACCESS
    void update_quick_list();
    std::vector<std::string> quick_access_paths(); 

    // PROGRAM
    void init();
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
