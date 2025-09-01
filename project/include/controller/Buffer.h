/**
 * @file Buffer.h
 * @author Angie Granda
 * @brief Contains Buffer class for storing path's documents. 
 * @date 2025-08-30
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef _CFD_BUFFER_H_
#define _CFD_BUFFER_H_
#include "Constants.h"
#include <string>
#include <vector>
#include <filesystem>

/**
 * @class Buffer
 * @brief Resposible for storing the directories and file names for a given filesystem path.
 * @details
 * - Scans a given filesystem path and separates its contents into directories and files.
 * - Encapsulation of the vectors containing the directories and files by returning constant 
 * references so it is guaranteed that the vectors will not change the contents during the execution of the program.
 */
class Buffer {

    private:
    std::vector<std::string> m_directories;
    std::vector<std::string> m_files;
    void fill_buffer(const fs::path& path); 
    
    public:
    Buffer(const fs::path& path);
    inline const std::vector<std::string>& get_directories() const { return m_directories; }
    inline const std::vector<std::string>& get_files() const { return m_files; }
    inline bool has_directories() { return m_directories.size() > 0; }
};

#endif