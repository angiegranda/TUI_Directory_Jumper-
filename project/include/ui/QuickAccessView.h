/**
 * @file QuickAccessView.h
 * @author Angie Granda
 * @brief Prepares and formats quick access data for terminal display.
 * @date 2025-08-31
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef _CFD_QUICKACCESS_VIEW_H_
#define _CFD_QUICKACCESS_VIEW_H_
#include "Constants.h"
#include "TerminalConstants.h"
#include "UI.h"
#include "QuickAccess.h"
#include <string>
#include <tuple>

/**
 * @class QuickAccessView
 * @brief The QuickAccessView class manages the visualization of top most relevant 
 * paths for rendering in the UI.
 * @details
 * - For paths not fitting the terminal width, it converts the absolute path into relative
 * preserving as much information of the parent paths as possible.
 * - Prepares headers, footers, and content rows according to quick access constants.
 * - Delegates final rendering to the static `UI` class once formatting is
 *   complete.
 * @see UI
 * @see Constants.h
 */

class QuickAccessView {
    private:
    static std::string path_length_constraints(const std::string& path, const std::size_t width);
    static std::string get_header_or_footer(const char* text, const std::size_t width);
    public: 
    static void render_window(const std::vector<PathInfo>& list, std::size_t pos); 
};

#endif