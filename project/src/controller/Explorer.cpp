#include "Explorer.h"
#include <stdexcept>

// ---------------------- PRIVATE -------------------

std::shared_ptr<Buffer> Explorer::create_buffer(const fs::path& path) {
    try {
        return std::make_shared<Buffer>(path);
    } catch (const std::bad_alloc& e) {
        throw std::runtime_error("Could not allocate dynamically, run out of memory");
    }
}

void Explorer::set_parent_buffer() {
    if (m_displayed_path == m_displayed_path.parent_path()){ 
        m_parent_buffer = nullptr;
    } else {
        m_parent_buffer = create_buffer(m_displayed_path.parent_path()); 
    }
}
   
void Explorer::set_child_buffer() {
    if (m_curr_type == DOC_TYPE::DIRECTORY) {
        m_child_buffer = create_buffer(m_displayed_path / m_current_buffer->get_directories()[m_curr_pos]);
    } else {
        m_child_buffer = nullptr;
    }
}

void Explorer::init() {
    m_curr_pos = 0;
    m_curr_type = m_current_buffer->has_directories() ? DOC_TYPE::DIRECTORY : DOC_TYPE::FILE_ENTRY;
    m_curr_total_options = m_current_buffer->get_directories().size() + m_current_buffer->get_files().size();
    set_parent_buffer();
    set_child_buffer();
}


// ---------------------- PUBLIC -------------------

Explorer::Explorer(const std::string& current_dir):
    m_displayed_path(current_dir),    
    m_current_buffer(create_buffer(m_displayed_path)) {
    init();
}
    
Explorer::Explorer(fs::path current_path):
    m_displayed_path(current_path),
    m_current_buffer(create_buffer(m_displayed_path)) {
    init();
}

std::size_t Explorer::get_parent_dir_pos() const {
    if (!m_parent_buffer) { 
        return 0;
    }
    std::size_t curr_pos = 0;
    std::string parent_selected_dir = m_displayed_path.filename().string();
    for (auto&& entry : m_parent_buffer->get_directories()) {
        if (entry == parent_selected_dir){
            break;
        }
        ++curr_pos;
    }
    return curr_pos;
}

fs::path Explorer::get_curr_displayed_dir() const {
    return m_displayed_path;
}

fs::path Explorer::get_valid_selected_dir() const {
    if (m_curr_type == DOC_TYPE::FILE_ENTRY) {
        return m_displayed_path;
    }
    return m_displayed_path / m_current_buffer->get_directories()[m_curr_pos];
}

std::size_t Explorer::get_curr_pos() const {
    return m_curr_pos;
}

void Explorer::move_foward() {
    if  (m_curr_type == DOC_TYPE::FILE_ENTRY || m_child_buffer == nullptr ||
        (m_child_buffer != nullptr && 
        m_child_buffer->get_files().size() + m_child_buffer->get_directories().size() == 0)) {
        return;
    }
    std::string next_doc = m_current_buffer->get_directories()[m_curr_pos];
    m_parent_buffer = m_current_buffer;
    m_current_buffer = m_child_buffer;
    m_displayed_path = m_displayed_path / next_doc;
    m_curr_total_options = m_current_buffer->get_directories().size() + m_current_buffer->get_files().size();
    m_curr_pos = 0;
    m_curr_type = m_current_buffer->has_directories() ? DOC_TYPE::DIRECTORY : DOC_TYPE::FILE_ENTRY;
    set_child_buffer(); 
}

// parent is defined, current is empty, child is null 
// but then parent is nullptr 
void Explorer::move_backward() {
    if (m_parent_buffer == nullptr) {
        return;
    }
    m_curr_pos = get_parent_dir_pos();
    m_child_buffer = m_current_buffer;
    m_current_buffer = m_parent_buffer;
    m_curr_total_options = m_current_buffer->get_directories().size() + m_current_buffer->get_files().size();
    m_displayed_path = m_displayed_path.parent_path();
    m_curr_type = DOC_TYPE::DIRECTORY;
    set_parent_buffer(); 
}

void Explorer::move_down() {
    if (m_curr_total_options > 0) { 
        m_curr_pos = (m_curr_pos + 1) % m_curr_total_options;
        std::size_t dirs_size = m_current_buffer->get_directories().size();
        if (m_curr_pos < dirs_size) {
            m_curr_type = DOC_TYPE::DIRECTORY; 
            std::string doc = m_current_buffer->get_directories()[m_curr_pos];
            m_child_buffer = create_buffer(m_displayed_path / doc);
        } else {
            m_curr_type = DOC_TYPE::FILE_ENTRY;
            m_child_buffer = nullptr;
        }
    }
}

void Explorer::move_up() {
    if (m_curr_total_options > 0) { 
        m_curr_pos = (m_curr_pos == 0) ? m_curr_total_options - 1 : m_curr_pos - 1;
        std::size_t dirs_size = m_current_buffer->get_directories().size();
        m_curr_type = m_curr_pos < dirs_size ? DOC_TYPE::DIRECTORY : DOC_TYPE::FILE_ENTRY;
        if (m_curr_pos < dirs_size) {
            m_curr_type = DOC_TYPE::DIRECTORY; 
            std::string doc = m_current_buffer->get_directories()[m_curr_pos];
            m_child_buffer = create_buffer(m_displayed_path / doc);
        } else {
            m_curr_type = DOC_TYPE::FILE_ENTRY;
            m_child_buffer = nullptr;
        }
    }
}