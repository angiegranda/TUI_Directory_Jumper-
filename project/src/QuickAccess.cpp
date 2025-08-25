#include "QuickAccess.h"
#include <fstream>
#include <sstream>
#include <exception>
#include <cmath>
#include <algorithm>
#include <stdexcept>

#if defined(__linux__)
#include <unistd.h>
#include <limits.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#else // WIN32_
#include <filesystem>
#include <windows.h>
#endif

fs::path QuickAccess::get_executable_directory() {
#if defined(__linux__)
    char buf[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf)-1);
    if (len == -1) throw std::runtime_error("Cannot read /proc/self/exe");
    buf[len] = '\0';
    return std::filesystem::path(buf).parent_path();
#elif defined(__APPLE__)
    char buf[PATH_MAX];
    uint32_t size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &size) != 0)
        throw std::runtime_error("Buffer too small for executable path");
    auto path = std::filesystem::path(buf);
    return std::filesystem::canonical(path).parent_path();
#else
    wchar_t buffer[MAX_PATH];
    DWORD len = GetModuleFileNameW(NULL, buffer, MAX_PATH);
    if (len == 0) throw std::runtime_error("Cannot get executable path");
    return std::filesystem::path(buffer).parent_path();
#endif
}

QuickAccess::QuickAccess() : file_path(get_executable_directory() / QUICK_ACCESS_FILE) {
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
