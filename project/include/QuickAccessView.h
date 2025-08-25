#ifndef _CFD_QUICKACCESS_VIEW_H_
#define _CFD_QUICKACCESS_VIEW_H_
#include "Constants.h"
#include "TerminalConstants.h"
#include "UI.h"
#include <string>
#include <tuple>

class QuickAccessView {
    private:
    std::string path_length_constraints(const std::string& path, const std::size_t width);
    std::string get_header_or_footer(const char* text, const std::size_t width);
    public: 
    void render_window(std::vector<std::string> list, std::size_t pos); 
};

#endif