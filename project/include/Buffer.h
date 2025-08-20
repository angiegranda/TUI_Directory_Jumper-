#ifndef _CFD_BUFFER_H_
#define _CFD_BUFFER_H_
#include "Alias.h"
#include <string>
#include <vector>
#include <filesystem>

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