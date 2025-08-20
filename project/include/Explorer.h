#ifndef _CFD_EXPLORER_H_
#define _CFD_EXPLORER_H_
#include <memory>
#include <optional>
#include <tuple>
#include "Buffer.h"


class Explorer {
private:

    fs::path m_current_path;
    std::optional<std::string> m_selected_dir;

    // buffers are nullpointers when empty 
    std::shared_ptr<Buffer> m_current_buffer;
    std::shared_ptr<Buffer> m_parent_buffer;
    std::shared_ptr<Buffer> m_child_buffer;

    void set_parent_buffer();
    void set_child_buffer();

public:

    Explorer(const std::string& current_dir);
    Explorer(fs::path current_path);

    // Needed for IU 

    inline const std::shared_ptr<Buffer> get_parent_buffer() const {return m_parent_buffer; }
    inline const std::shared_ptr<Buffer> get_current_buffer() const {return m_current_buffer; }
    inline const std::shared_ptr<Buffer> get_child_buffer() const {return m_child_buffer; }

    std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t>  get_parent_data() const;
    std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t> get_current_data() const;
    inline std::tuple<const std::shared_ptr<Buffer>, bool, std::size_t> get_child_data() const { return std::make_tuple(m_child_buffer, false, 0); }

    inline const std::optional<std::string>& get_curr_selected_dir() const {return m_selected_dir; }
    std::string get_parent_selected_dir() const; // this is more complex, possible null 


    // Actions
    bool move_foward(); // if child_buffer is not empty
    bool move_backward(); // if parent_buffer is not empty
    void update_child_buffer(const std::string& selected_dir);

    // // DEBUG FUNCTIONS 
    // void debug_options();
    // void display_debug();
    // void display_buffer_debug(fs::path path, std::shared_ptr<Buffer> buffer, const std::optional<std::string>& curr_selected_dir);

};


#endif 
