#include "WindowsResizeMonitor.h"

#include <chrono>
#include <tuple>
#include "UI.h"

// for linux and mac this can work but it is not optmal 

WindowsResizeMonitor::~WindowsResizeMonitor() {
    stop();
}

void WindowsResizeMonitor::start() {
    std::lock_guard<std::mutex> lock(_mutex); // Thid ensures that only one thread at a time can enter that a time

    if (_is_running) { return; }
    _is_running = true;
    _worker = std::thread(std::bind(&WindowsResizeMonitor::m_execute, this));
}

void WindowsResizeMonitor::stop() {
    std::lock_guard<std::mutex> lock(_mutex);

    if (!_is_running) { return; }
    _is_running = false;
    if (_worker.joinable()) {
        _worker.join(); // wait until worker really exits
    }
}

void WindowsResizeMonitor::m_execute() const {
    std::tuple<int, int> last_size = UI::get_terminal_size();
    std::tuple<int, int> size;
    while (_is_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        size = UI::get_terminal_size();
        if (size != last_size) {
            last_size = size;
            UI::window_resize_handler();
        }
    }
}
