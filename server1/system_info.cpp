#include "system_info.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <X11/extensions/Xinerama.h>
#include <X11/Xlib.h>
#include <sstream>
#include <ctime>

std::string get_window_size() {
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    std::ostringstream oss;
    oss << size.ws_row << "x" << size.ws_col;
    return oss.str();
}

int get_monitor_count() {
    Display* display = XOpenDisplay(NULL);
    if (!display) return -1;

    int num_monitors = 0;
    if (XineramaIsActive(display)) {
        XineramaScreenInfo* screens = XineramaQueryScreens(display, &num_monitors);
        if (screens) XFree(screens);
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