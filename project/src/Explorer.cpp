#include "Explorer.h"

Explorer::Explorer(const std::string& current_dir):
    m_current_path(current_dir),    
    m_current_buffer(std::make_shared<Buffer>(m_current_path)) {
        set_parent_buffer();
        set_child_buffer();
    }

Explorer::Explorer(fs::path current_path):
    m_current_path(current_path),
    m_current_buffer(std::make_shared<Buffer>(m_current_path)) {
        set_parent_buffer();
        set_child_buffer();
    }
  
fs::path Explorer::get_current_path() const {
    return m_current_path;
}

fs::path Explorer::get_path_to_visit() const {
    if (!m_child_buffer) { 
        return m_current_path;
    }
    return m_current_path / m_selected_dir.value();
}

std::size_t Explorer::get_parent_dir_pos() const {
    if (!m_parent_buffer) { 
        return 0;
    }
    std::size_t curr_pos = 0;
    std::string parent_selected_dir = m_current_path.filename().string();
    for (auto&& entry : m_parent_buffer->get_directories()) {
        if (entry == parent_selected_dir){
            break;
        }
        ++curr_pos;
    }
    return curr_pos;
}

void Explorer::set_parent_buffer() {
    if (m_current_path == m_current_path.parent_path()){ 
        m_parent_buffer = nullptr;
        return;
    }
    m_parent_buffer = std::make_shared<Buffer>(m_current_path.parent_path());
}
   
void Explorer::set_child_buffer() {
     if (!m_current_buffer->has_directories()) {
        m_child_buffer = nullptr;
        m_selected_dir = std::nullopt;
        return;
    }
    m_selected_dir = *(m_current_buffer->get_directories().begin()); // getting the first directory
    update_child_buffer(m_selected_dir.value());
}

void Explorer::update_child_buffer(const std::string& dir) {
    m_selected_dir = dir;
    fs::path child_path = m_current_path / dir;
    m_child_buffer = std::make_shared<Buffer>(child_path);
}

void Explorer::move_foward() {
    if (m_child_buffer == nullptr) { return; }
    m_parent_buffer = m_child_buffer ? m_current_buffer : m_parent_buffer;
    m_current_buffer = m_child_buffer ? m_child_buffer : m_current_buffer ;
    m_current_path = m_child_buffer ? m_current_path / m_selected_dir.value() : m_current_path;
    set_child_buffer();
}

void Explorer::move_backward() {
    if (m_parent_buffer == nullptr) { return; }
    m_selected_dir = m_parent_buffer ? m_current_path.filename().string() : m_selected_dir;
    m_current_buffer = m_parent_buffer ? m_parent_buffer : m_current_buffer;
    m_current_path = m_parent_buffer ? m_current_path.parent_path() : m_current_path;
    update_child_buffer(m_selected_dir.value());
    set_parent_buffer();
}
