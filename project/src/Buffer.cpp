#include "Buffer.h"
#include <iostream> // when errors cannot happend then it is not needed
#include <exception>

Buffer::Buffer(const fs::path& path) { fill_buffer(path); } 

void Buffer::fill_buffer(const fs::path& path){
    m_directories.clear();
    m_files.clear();
    try {
        if (fs::exists(path) && fs::is_directory(path)) {
            for (auto&& entry : fs::directory_iterator(path)) {
                if (fs::is_directory(entry)){
                    m_directories.emplace_back(entry.path().filename().string());
                }
                else{
                    m_files.emplace_back(entry.path().filename().string());
                }
            }

            std::sort(m_directories.begin(), m_directories.end(), 
                [](std::string a, std::string b) { return a < b;}); 
            std::sort(m_files.begin(), m_files.end(), 
                [](std::string a, std::string b) { return a < b;}); 
        } 
        else {
            std::cout << "Path not found!" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

