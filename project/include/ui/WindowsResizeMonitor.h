/**
 * @file WindowsResizeMonitor.h
 * @author Angie Granda
 * @brief Monitors terminal window resize events on Windows.
 * @date 2025-09-01
 * @copyright Copyright (c) 2025
 */
#ifndef _CFD_WINDOWS_RESIZE_MONITOR_H_
#define _CFD_WINDOWS_RESIZE_MONITOR_H_


#include <atomic>
#include <mutex>
#include <thread>

/**
 * @class WindowsResizeMonitor
 * @brief This class runs a background thread to detect changes in the terminal window size
 * @details 
 * - Launching a worker thread to continuously check the terminal size.
 * - Stopping the thread safely.
 * - Notifying UI or registered handlers when a resize occurs.
 * - All operations are thread-safe. Safety measures are:
 *      - A `std::mutex` protects `_is_running` and ensures only one thread can start/stop the monitor at a time.
 *      - A `_is_running` flag lets the worker thread exit gracefully instead of being forcefully killed.
 *      - The worker thread is joined in `stop()` and the destructor, so no thread is left running when the object is destroyed.
 */

class WindowsResizeMonitor {
private:
    std::thread _worker;
    std::atomic<bool> _is_running;
    std::mutex _mutex;
    void m_execute() const;
    
public:
    ~WindowsResizeMonitor();
    void start();
    void stop();
};


#endif 
