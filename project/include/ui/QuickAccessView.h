#ifndef _CFD_QUICKACCESS_VIEW_H_
#define _CFD_QUICKACCESS_VIEW_H_
#include "Constants.h"
#include "TerminalConstants.h"
#include "UI.h"
#include "QuickAccess.h"
#include <string>
#include <tuple>

class QuickAccessView {
    private:
    static std::string path_length_constraints(const std::string& path, const std::size_t width);
    static std::string get_header_or_footer(const char* text, const std::size_t width);
    public: 
    static void render_window(const std::vector<PathInfo>& list, std::size_t pos); 
};

#endif