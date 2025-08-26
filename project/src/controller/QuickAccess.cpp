#include "QuickAccess.h"
#include <fstream> // ifstream ofstream
#include <sstream> // stringstream 
#include <cmath> // std::pow
#include <algorithm> // std::sort
#include <cstdlib> // std::getenv
#include <exception>
#include <iostream>

void QuickAccess::set_path() {
    const char* dir;
    try {
#ifndef _WIN32
        dir = std::getenv(GLOBAL_ENV_PATH_POSIX1);
        if (!dir) {
            dir = std::getenv(GLOBAL_ENV_PATH_POSIX2); 
        }
#else
        dir = std::getenv(GLOBAL_ENV_PATH_WIND);
#endif
        fs::path env_path(dir);
        fs::path project_path = env_path / QUICK_ACCESS_PROJECT_FOLDER;
        fs::create_directory(project_path);
        m_file_path = project_path/ QUICK_ACCESS_FILE;
        m_state = true;
    } catch (const std::exception&) {
        m_state = false;
    }
}

PathInfo QuickAccess::create_pathinfo(const std::string& data) {
    PathInfo p;
    std::stringstream ss(data);
    std::string arg;
    std::getline(ss, arg, CSV_SEPARATOR);
    p.m_path = arg;
    p.m_score = 0.0;
    while (std::getline(ss, arg, CSV_SEPARATOR)) {
        std::size_t visit_time = std::stoul(arg);
        p.m_visits.emplace_back(visit_time);
        p.m_score += std::pow(LAMBDA, static_cast<double>(m_total_visits - visit_time));
    }
    return p;
}

std::fstream QuickAccess::open_file() {
    std::fstream file(m_file_path);
    if (!file.is_open()) {
        std::ofstream create_file(m_file_path);
        create_file << 0 << NEW_LINE;
        create_file.close();
        file.open(m_file_path);
    }
    return file;
}

QuickAccess::QuickAccess() {
    set_path();
    if (!m_state) {return; }
    m_data.clear();
    std::fstream file = open_file();
    std::size_t lines_count = 0; 
    std::string line;
    try {
        while (std::getline(file, line)) {
            ++lines_count;
            if (lines_count == 1) {
                m_total_visits = std::stoul(line);
                continue;
            }
            m_data.emplace_back(create_pathinfo(line));
        }
        if (lines_count > 1) {
            std::sort(
                m_data.begin(), m_data.end(), 
                [](const PathInfo& a, const PathInfo& b) 
                { return a.m_score > b.m_score; }
            );
        }
    } catch (const std::exception&) { // WRONG FORMAT 
        m_data.clear();
        m_total_visits = 0;
    }
    file.close();
}

bool QuickAccess::get_state() {
    return m_state;
}

void QuickAccess::save_data() {
    std::ofstream file(m_file_path, std::ios::out | std::ios::trunc);
    file << m_total_visits << NEW_LINE;
    for (auto&& path_entry : m_data) {
        std::ostringstream oss;
        oss << path_entry.m_path;
        for (auto&& time : path_entry.m_visits) {
            oss << CSV_SEPARATOR << time; 
        }
        oss << NEW_LINE; 
        file << oss.str();
    }
}
