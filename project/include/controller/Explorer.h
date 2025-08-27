#ifndef _CFD_EXPLORER_H_
#define _CFD_EXPLORER_H_
#include "Buffer.h"
#include <memory>
#include <optional>
#include <tuple>

enum class DOC_TYPE { DIRECTORY, FILE };

// TODO: std::bad_alloc

class Explorer {

private:

    DOC_TYPE m_curr_type;
    fs::path m_displayed_path;
    std::size_t m_curr_pos;
    std::size_t m_curr_total_options;
    std::shared_ptr<Buffer> m_current_buffer;
    std::shared_ptr<Buffer> m_parent_buffer;
    std::shared_ptr<Buffer> m_child_buffer;

    void init();
    void set_parent_buffer();
    void set_child_buffer();
    std::shared_ptr<Buffer> create_buffer(const std::string& path);

public:

    Explorer(const std::string& current_dir);
    Explorer(fs::path current_path);

    inline const std::shared_ptr<Buffer> get_parent_buffer() const {return m_parent_buffer; }
    inline const std::shared_ptr<Buffer> get_current_buffer() const {return m_current_buffer; }
    inline const std::shared_ptr<Buffer> get_child_buffer() const {return m_child_buffer; }


    std::size_t get_parent_dir_pos() const;
    fs::path get_valid_selected_dir() const;
    fs::path get_curr_displayed_dir() const;
    std::size_t get_curr_pos() const; 

    void move_foward(); 
    void move_backward(); 
    //void update_child_buffer(const std::string& selected_dir);
    void move_down();
    void move_up();

};

#endif 
