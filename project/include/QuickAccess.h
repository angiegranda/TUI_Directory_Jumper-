#ifndef _CFD_QUICKACCESS_H_
#define _CFD_QUICKACCESS_H_
#include "Constants.h"
#include <string>
#include <vector>

struct PathInfo {
    std::string m_path;
    std::vector<std::size_t> m_visits;
    double m_score;
};

class QuickAccess {
    private:
    std::string get_executable_directory();
    public:
    // m_total_visits == 0 iff m_data == 0 
    const fs::path file_path;
    std::size_t m_total_visits; // 0
    std::vector<PathInfo> m_data; // empty  
    QuickAccess();
    void save_data();
};


#endif