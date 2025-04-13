#include "system_info.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <X11/extensions/Xinerama.h>
#include <X11/Xlib.h>
#include <sstream>
#include <ctime>
#include <iostream>

std::string get_window_size() {
    // Check if the terminal device is available (if we're running in a Docker container)
    struct winsize size;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == -1) {
        // If ioctl fails, return default size (assumed 24x80)
        return "24x80"; 
    }

    // If ioctl succeeds, return the actual window size
    std::ostringstream oss;
    oss << size.ws_row << "x" << size.ws_col;
    return oss.str();
}


int get_monitor_count() {
    Display* display = XOpenDisplay(NULL);
    if (!display) return 1;

    int count = 1; // default to 1 monitor
    if (XineramaIsActive(display)) {
        int num;
        XineramaScreenInfo* screens = XineramaQueryScreens(display, &num);
        if (screens) {
            count = num;
            XFree(screens);
        }
    }

    XCloseDisplay(display);
    return count;
}

std::string get_current_time() {
    std::time_t now = std::time(0);
    std::tm* timeinfo = std::localtime(&now);
    char buffer[9]; // hh:mm:ss
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    return std::string(buffer);
}

std::string get_system_info() {
    std::ostringstream info;
    info << "Window size: " << get_window_size() << "\n";
    info << "Monitors: " << get_monitor_count() << "\n";
    info << "Time: " << get_current_time();
    return info.str();
}