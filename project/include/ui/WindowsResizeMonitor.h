#ifndef _CFD_WINDOWS_RESIZE_MONITOR_H_
#define _CFD_WINDOWS_RESIZE_MONITOR_H_


#include <atomic>
#include <mutex>
#include <thread>


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
