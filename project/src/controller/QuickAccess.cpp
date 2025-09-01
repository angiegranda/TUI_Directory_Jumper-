#include "QuickAccess.h"
#include <fstream> // ifstream ofstream
#include <sstream> // stringstream 
#include <cmath> // std::pow
#include <algorithm> // std::sort
#include <cstdlib> // std::getenv
#include <exception>
#include <iostream>

/**
 * @brief Sets up the quick access file path.
 * @details
 * - Reads the base directory from environment variables depending on the platform.
 *   - POSIX: tries GLOBAL_ENV_PATH_POSIX1, then GLOBAL_ENV_PATH_POSIX2.
 *   - Windows: uses GLOBAL_ENV_PATH_WIND.
 * - Creates the project folder inside that base directory if it doesn't exist.
 * - Defines the path to the quick access file and marks the state as valid.
 * - If any exception occurs state is set to false. Controller checks the state at initialization, 
 * if false then the program stops. 
 * @see Constants.h
 */
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

/**
 * @brief Parses a line of csv quick access file into a PathInfo object.
 * @details
 * - The first entry in the line is the path.
 * - Remaining entries are visit timestamps.
 * - Each visit contributes to the score using an exponential decay factor `LAMBDA`.
 * @param data A CSV-formatted string containing path and visit timestamps.
 * @return A populated PathInfo instance with path, visits, and computed score.
 * @see PathInfo
 * @see Constants.h
 */
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

/**
 * @brief Opens the quick access file.
 * @details
 * - If the file does not exist, it is created with a default line `0` which indicates the current timestamp.
 * @return A std::fstream object for the quick access file.
 */
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

/**
 * @brief Constructs a QuickAccess object and loads data.
 * @details
 * - Calls @ref set_path to initialize environment and file path.
 * - If state is invalid, initialization stops.
 * - Otherwise:
 *   - Opens the quick access file.
 *   - Reads previous timestamp from the first line.
 *   - Parses subsequent lines into PathInfo objects with @ref create_pathinfo.
 *   - Sorts entries in descending order of score.
 * - If the file is corrupted or has an invalid format, data is reset and total visits set to 0.
 */
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

/**
 * @brief Returns the current state which is true if initialized successfully, false otherwise.
 */
bool QuickAccess::get_state() {
    return m_state;
}

/**
 * @brief Saves the current QuickAccess data to the file.
 * @details
 * - Overwrites the quick access file.
 * - First line contains total visits.
 * - Each subsequent line contains a path followed by its visit timestamps CSV formatted.
 */
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
