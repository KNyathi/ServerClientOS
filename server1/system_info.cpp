#include "system_info.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <X11/extensions/Xinerama.h>
#include <X11/Xlib.h>
#include <sstream>
#include <ctime>
#include <iostream>

std::string get_window_size() {
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    std::ostringstream oss;
    oss << size.ws_row << "x" << size.ws_col;
    return oss.str();
}



int get_monitor_count() {
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "Error: Unable to open X display\n";
        return -1;  // Return -1 to indicate an error opening the display
    }

    int num_monitors = 0;
    
    // Check if Xinerama is active (used in multi-monitor setups)
    if (XineramaIsActive(display)) {
        int screen_count = 0;
        XineramaScreenInfo* screens = XineramaQueryScreens(display, &screen_count);
        if (screens) {
            num_monitors = screen_count;
            XFree(screens);
        } else {
            std::cerr << "Error: Unable to query Xinerama screens\n";
        }
    } else {
        // If Xinerama is not active, fallback to ScreenCount
        num_monitors = ScreenCount(display);
    }

    XCloseDisplay(display);
    return num_monitors;
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