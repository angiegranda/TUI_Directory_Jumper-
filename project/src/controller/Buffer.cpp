#include "Buffer.h"
#include <iostream> 
#include <exception>
#include <algorithm>
#include <fstream>

Buffer::Buffer(const fs::path& path) { fill_buffer(path); } 

void Buffer::fill_buffer(const fs::path& path){
    m_directories.clear();
    m_files.clear();
    try {
        if (fs::exists(path) && fs::is_directory(path)) {
            for (auto&& entry : fs::directory_iterator(path)) {
                try {
                    if (fs::is_directory(entry)){
                        m_directories.emplace_back(entry.path().filename().string());
                    }
                    else if (fs::is_regular_file(entry)) {
                        std::ifstream file(entry.path(), std::ios::binary);
                        if (file.is_open()) {
                            m_files.emplace_back(entry.path().filename().string());
                        }
                    }
                }
                catch (const std::exception&) {
                    continue;
                }
            }
            std::sort(m_directories.begin(), m_directories.end());
            std::sort(m_files.begin(), m_files.end()); 
        } 
    }
    catch (const std::exception&) {
        m_directories.clear();
        m_files.clear();
    }
}

