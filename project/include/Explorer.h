#ifndef _CFD_EXPLORER_H_
#define _CFD_EXPLORER_H_
#include "Buffer.h"
#include <memory>
#include <optional>
#include <tuple>


class Explorer {
private:
 
    fs::path m_current_path;
    std::optional<std::string> m_selected_dir;
    std::shared_ptr<Buffer> m_current_buffer;
    std::shared_ptr<Buffer> m_parent_buffer;
    std::shared_ptr<Buffer> m_child_buffer;

    void set_parent_buffer();
    void set_child_buffer();

public:

    Explorer(const std::string& current_dir);
    Explorer(fs::path current_path);

    inline const std::shared_ptr<Buffer> get_parent_buffer() const {return m_parent_buffer; }
    inline const std::shared_ptr<Buffer> get_current_buffer() const {return m_current_buffer; }
    inline const std::shared_ptr<Buffer> get_child_buffer() const {return m_child_buffer; }

    std::size_t get_parent_dir_pos() const;
    fs::path get_path_to_visit() const;
    fs::path get_current_path() const;

    void move_foward(); 
    void move_backward(); 
    void update_child_buffer(const std::string& selected_dir);

};

#endif 
