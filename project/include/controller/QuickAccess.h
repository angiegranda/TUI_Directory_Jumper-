/**
 * @file QuickAccess.h
 * @author Angie Granda
 * @brief Manages persistent storage and scoring of frequently visited paths.
 * @date 2025-08-31
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef _CFD_QUICKACCESS_H_
#define _CFD_QUICKACCESS_H_
#include "Constants.h"
#include <string>
#include <vector>

/**
 * @class PathInfo
 * @brief Stores a path together with a vector with the times it was previously visited.
 * Used by QuickAccess class.
 * @see QuickAccess
 */
struct PathInfo {
    std::string m_path;
    std::vector<std::size_t> m_visits;
    double m_score = 0.0;
};

/**
 * @class QuickAccess
 * @brief This class handles the file that records how often and how recently each path was visited.
 * @details 
 * - Opens and reads the Quick Access data file.
 * - Uses `PathInfo` objects to store metadata for each tracked path.
 * - It implements an algorithm that balances:
 *      - **Recency**: recently accessed paths are prioritized.
 *      - **Frequency**: frequently accessed paths gain higher weight.
 * - Exposes a public vector of `PathInfo` instances for use by the Controller.
 * - On program termination, truncates the file and overwrites it with updated
 *   visit data (if any path was selected).
 * @see PathInfo
 * @see Controller 
 */

class QuickAccess {
    private:
    fs::path m_file_path;
    bool m_state;
    void set_path();
    PathInfo create_pathinfo(const std::string& data);
    std::fstream open_file();
    public:
    std::size_t m_total_visits; 
    std::vector<PathInfo> m_data;
    QuickAccess();
    void save_data();
    bool get_state();
};


#endif