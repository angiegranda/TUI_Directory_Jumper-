#ifndef _CFD_QUICKACCESS_H_
#define _CFD_QUICKACCESS_H_
#include "Constants.h"
#include <string>
#include <vector>

struct PathInfo {
    std::string m_path;
    std::vector<std::size_t> m_visits;
    double m_score = 0.0;
};

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