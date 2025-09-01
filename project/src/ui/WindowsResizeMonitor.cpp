#include "WindowsResizeMonitor.h"

#include <chrono>
#include <tuple>
#include "UI.h"

// DEF race condition: two or more threads try to access or modify the same data at the same time 

/**
 * @brief Destructor. 
 * @details 
 * Ensures the background thread is stopped and joined before cleanup.
 * This prevents dangling threads when the object goes out of scope.
 */
WindowsResizeMonitor::~WindowsResizeMonitor() {
    stop();
}

/**
 * @brief Starts monitoring terminal size in a separate thread.
 * @details
 * - Uses a `std::mutex` lock to avoid race conditions.
 * - Does nothing if the monitor is already running.
 */
void WindowsResizeMonitor::start() {
    std::lock_guard<std::mutex> lock(_mutex); // Thid ensures that only one thread at a time can enter that a time

    if (_is_running) { return; }
    _is_running = true;
    _worker = std::thread(std::bind(&WindowsResizeMonitor::m_execute, this));
}

/**
 * @brief Stops the monitoring thread safely.
 * @details 
 * - Uses a `std::mutex` lock to avoid other threads accessing at the same time.
 * - Sets `_is_running = false` so the worker thread can exit naturally.
 * - Joins (wait until worker really exits) the worker thread if it is still running, ensuring safe cleanup.
 */
void WindowsResizeMonitor::stop() {
    std::lock_guard<std::mutex> lock(_mutex);

    if (!_is_running) { return; }
    _is_running = false;
    if (_worker.joinable()) {
        _worker.join(); 
    }
}

/**
 * @brief Background worker that checks for terminal resize events.
 * @details 
 * - Runs in a loop while `_is_running` is `true`.
 * - Sleeps for 200 ms between checks to avoid high CPU usage.
 * - Calls `UI::window_resize_handler()` when a size change is detected.
 * - Exits gracefully once `_is_running` is set to `false` by `stop()`.
 */
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
