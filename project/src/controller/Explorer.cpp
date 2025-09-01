#include "Explorer.h"
#include <stdexcept>

/**
 * @brief Given a path, it creates a buffer.
 * @param path filesystem path.
 * @throws std::bad_alloc if there is not enough memory for dynamic allocation. Controller will catch it. 
 */
std::shared_ptr<Buffer> Explorer::create_buffer(const fs::path& path) {
    try {
        return std::make_shared<Buffer>(path);
    } catch (const std::bad_alloc& e) {
        throw;
    }
}

/**
 * @brief Update parent buffer. 
 * @details
 * - If current path is equal to parent path, then we are at root.
 * - Else, then a buffer is created with the parent path. 
 * - This is used at initialization and backward move.
 * @throws std::bad_alloc if there is not enough memory for dynamic allocation.
 */
void Explorer::set_parent_buffer() {
    if (m_displayed_path == m_displayed_path.parent_path()){ 
        m_parent_buffer = nullptr;
    } else {
        m_parent_buffer = create_buffer(m_displayed_path.parent_path()); 
    }
}

/**
 * @brief Update child buffer. 
 * @details
 * - If the current document type is `FILE_ENTRY`, set the child buffer to nullptr.
 * - If it is a directory, create a buffer for the directory at the current cursor position (`m_curr_pos`).
 * - This is used at initialization and when moving forward.
 * @throws std::bad_alloc if there is not enough memory for dynamic allocation.
 */
void Explorer::set_child_buffer() {
    if (m_curr_type == DOC_TYPE::DIRECTORY) {
        m_child_buffer = create_buffer(m_displayed_path / m_current_buffer->get_directories()[m_curr_pos]);
    } else {
        m_child_buffer = nullptr;
    }
}

/**
 * @brief Initialization of members.
 * @details
 * - `m_curr_pos`: cursor initialized to 0, points to the first available entry 
 *   in [ directories + files ] of the current buffer.  
 * - `m_curr_type`: type of the entry at `m_curr_pos` (directory or file).
 * - `m_curr_total_options`: total number of selectable entries (directories + files).
 * - Calls @ref set_parent_buffer and @ref set_child_buffer to set up navigation buffers.
 */
void Explorer::init() {
    m_curr_pos = 0;
    m_curr_type = m_current_buffer->has_directories() ? DOC_TYPE::DIRECTORY : DOC_TYPE::FILE_ENTRY;
    m_curr_total_options = m_current_buffer->get_directories().size() + m_current_buffer->get_files().size();
    set_parent_buffer();
    set_child_buffer();
}

/**
 * @brief Constructor with a specific path. 
 * @details 
 * - the string can be used as a parameter of a filesystem::path object.
 * - Used mostly for debugging since the program needs the path from where it is being called.
 * @throws std::bad_alloc if there is not enough memory for dynamic allocation.
 */
Explorer::Explorer(const std::string& current_dir):
    m_displayed_path(current_dir),    
    m_current_buffer(create_buffer(m_displayed_path)) {
    init();
}

/**
 * @brief Constructor used in Main.cpp.
 * @details
 * - `m_displayed_path` is the path's contents shown in the middle column. The middle column for $SOME_PATH will contain $SOME_PATH/file.txt (if it exists).
 * - `m_current_buffer` contains the directories and files of the middle column which is the one the user interacts with.
 * @throws std::bad_alloc if there is not enough memory for dynamic allocation.
 */
Explorer::Explorer(fs::path current_path):
    m_displayed_path(current_path),
    m_current_buffer(create_buffer(m_displayed_path)) {
    init();
}

/**
 * @brief Finds and returns the position of the current directory inside its parent buffer. 
 * @details 
 * Used when moving backward so the cursor points to the directory we came from,
 * instead of always resetting to the first entry.
 */
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

/**
 * @brief Returns the selected path depending on the current type.
 * @details
 * - If the current type is `FILE_ENTRY`, returns the directory that contains it (`m_displayed_path`).
 * - If the current type is `DIRECTORY`, returns the full path of the selected directory 
 *   (`m_displayed_path / selected_directory`).
 */
fs::path Explorer::get_valid_selected_dir() const {
    if (m_curr_type == DOC_TYPE::FILE_ENTRY) {
        return m_displayed_path;
    }
    return m_displayed_path / m_current_buffer->get_directories()[m_curr_pos];
}

std::size_t Explorer::get_curr_pos() const {
    return m_curr_pos;
}

/**
 * @brief Moves the explorer into the currently selected directory.
 * 
 * Updates the current buffer to point to the child buffer if the current
 * selection is a directory with available entries. Also updates the path,
 * resets the cursor position, and prepares the next child buffer.
 * It does nothing if the current selection is a file, or if there are no child entries.
 */
void Explorer::move_forward() {
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

/**
 * @brief Moves the explorer back to the parent directory.
 * 
 * Restores the current buffer to its parent buffer, updates the displayed path,
 * and resets the selection to the parent directory’s position. If parent buffer is nullptr,
 * the function does nothing.
 */
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

/**
 * @brief Moves the selection cursor down to the next entry.
 * 
 * Advances the current position by one in the list of directories and files,
 * wrapping around when reaching the end. Updates the type (directory or file)
 * and prepares a new child buffer. 
 * @throws std::bad_alloc if there is not enough memory for dynamic allocation.
 */
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

/**
 * @brief Moves the selection cursor up to the previous entry.
 * 
 * Decrements the current position in the list of directories and files,
 * wrapping around to the last entry if at the beginning. Updates the type
 * (directory or file) and refreshes the child buffer. 
 * @throws std::bad_alloc if there is not enough memory for dynamic allocation.
 */
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