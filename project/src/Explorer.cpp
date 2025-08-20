#include "Explorer.h"
#include <iostream>

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

std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t>  Explorer::get_parent_data() const {
    if (!m_parent_buffer) { // no parent buffer 
        return std::make_tuple(nullptr, false, 0);
    }
    std::size_t curr_pos = 0;
    std::string parent_selected_dir = get_parent_selected_dir();
    for (auto&& entry : m_parent_buffer->get_directories()) {
        if (entry == parent_selected_dir){
            break;
        }
        ++curr_pos;
    }
    return std::make_tuple(m_parent_buffer, true, curr_pos);
}

std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t> Explorer::get_current_data() const {
    if (!m_selected_dir) { // current directory has no directories
        return std::make_tuple(m_current_buffer, false, 0);
    }
    std::size_t curr_pos = 0;
    for (auto&& entry : m_current_buffer->get_directories()) {
        if (entry == m_selected_dir.value()){
            break;
        }
        ++curr_pos;
    }
    return std::make_tuple(m_current_buffer, true, curr_pos);
}

std::string Explorer::get_parent_selected_dir() const {
    return m_current_path.filename().string(); // folder in the parent buffer 
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
    // assuming controller will give only a correct directory
    m_selected_dir = dir;
    fs::path child_path = m_current_path / dir;
    m_child_buffer = std::make_shared<Buffer>(child_path);
}

bool Explorer::move_foward() {
    m_parent_buffer = m_child_buffer ? m_current_buffer : m_parent_buffer;
    m_current_buffer = m_child_buffer ? m_child_buffer : m_current_buffer ;
    m_current_path =  m_child_buffer ? m_current_path / m_selected_dir.value() : m_current_path;
    set_child_buffer();
    return m_child_buffer ? true : false;
}

// True is there is a parent directory else False 
bool Explorer::move_backward(){
    m_selected_dir = m_parent_buffer ? get_parent_selected_dir() : m_selected_dir;
    m_current_buffer = m_parent_buffer? m_parent_buffer : m_current_buffer;
    m_current_path = m_parent_buffer ? m_current_path.parent_path() : m_current_path;
    update_child_buffer(m_selected_dir.value());
    set_parent_buffer();
    return m_parent_buffer ? true : false;
}


//DEBUG 


// void Explorer::display_buffer_debug(fs::path path, std::shared_ptr<Buffer> buffer, const std::optional<std::string>& curr_selected_dir) {
//     std::cout << "PATH: " <<  path.string() << std::endl;
//     for (auto&& entry: buffer->get_directories()) {
//         std::cout << "D: " << entry;
//         if  (curr_selected_dir && entry == *curr_selected_dir)
//             std::cout << " <--- selected";
//         std::cout << std::endl;
//     } 
//     for (auto&& entry: buffer->get_files()) {
//         std::cout << "F: " << entry << std::endl;
//     }    
// }

// void Explorer::display_debug() {
//     std::cout << std::endl;
//     std::cout << "PARENT" << std::endl;
//     std::cout << std::endl;
//     if (m_parent_buffer) {
//         display_buffer_debug(_current_path.parent_path(), m_parent_buffer, get_parent_selected_dir());
//     }
    
//     std::cout << std::endl;
//     std::cout << "CURRENT" << std::endl;
//     std::cout << std::endl;
//     display_buffer_debug(_current_path, _current_buffer, get_curr_selected_dir());
//     std::cout << std::endl;
//     std::cout << "CHILD" << std::endl;
//     std::cout << std::endl;
//     if (m_child_buffer) {
//         display_buffer_debug(_current_path/_selected_dir.value(), m_child_buffer, std::nullopt);
//     }
// }

// void Explorer::debug_options() { 

//     std::cout << std::endl;
//     display_debug();
//     std::cout << std::endl; 

//     bool move = move_backward();
//     std::cout << (move ? "Incorrect" : "Correct") << std::endl;

//     std::cout << std::endl;
//     display_debug();
//     std::cout << std::endl;

//     move = move_backward();
//     std::cout << (move ? "Incorrect" : "Correct") << std::endl;

//     std::cout << std::endl;
//     display_debug();
//     std::cout << std::endl;

//     move = move_foward();
//     std::cout << (move ? "Correct" : "Incorrect") << std::endl;
    
//     std::cout << std::endl;
//     display_debug();
//     std::cout << std::endl;

// }