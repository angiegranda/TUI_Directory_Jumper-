#include "QuickAccess.h"
#include <fstream>
#include <sstream>
#include <exception>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>

#if defined(__linux__)
#include <unistd.h>
#include <limits.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#else // WIN32_
#include <filesystem>
#include <windows.h>
#endif


std::string QuickAccess::get_executable_directory() {
    const char* dir;

#ifndef _WIN32
    dir = std::getenv("XDG_DATA_HOME"); // Preferred for Linux
    if (!dir) {
        dir = std::getenv("HOME"); // Fallback to HOME
    }
#else
    dir = std::getenv("APPDATA"); // For Windows
#endif

    // Check if the environment variable is set
    if (dir) {
        std::filesystem::path dataPath = std::filesystem::path(dir) / QUICK_ACCESS_FILE; // Append a subdirectory
        return dataPath.string(); // Convert to std::string
    }
    else {
        return ""; // Return an empty string if not found
    }
}

QuickAccess::QuickAccess() : file_path(get_executable_directory()) {
    m_data.clear();
    std::fstream file(file_path);
    if (!file.is_open()) {
        std::ofstream create_file(file_path);
        create_file << 0 << NEW_LINE;
        create_file.close();
        file.open(file_path);
    }
    std::size_t lines_count = 0; 
    std::string line;
    try {
        while (std::getline(file, line)) {
            if (lines_count == 0) {
                m_total_visits = std::stoul(line);
                ++lines_count;
                continue;
            }
            PathInfo p;
            std::stringstream ss(line);
            std::string arg;
            std::getline(ss, arg, CSV_SEPARATOR);
            p.m_path = arg;
            p.m_score = 0.0;
            while (std::getline(ss, arg, CSV_SEPARATOR)) {
                std::size_t visit_time = std::stoul(arg);
                p.m_visits.emplace_back(visit_time);
                p.m_score += std::pow(LAMBDA, static_cast<double>(m_total_visits - visit_time));
            }
            m_data.emplace_back(p);
            ++lines_count;
        }
        if (lines_count > 1) {
            std::sort(
                m_data.begin(), m_data.end(), 
                [](const PathInfo& a, const PathInfo& b) 
                { return a.m_score > b.m_score; }
            );
        }
    } catch (const std::exception&) {
        m_data.clear();
    }
    file.close();
}

void QuickAccess::save_data() {
    std::ofstream file(file_path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        return;
    }
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
