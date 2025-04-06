#include "system_info.h"
#include <thread>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>

int get_thread_count() {
    return std::thread::hardware_concurrency();
}

int get_process_count() {
    // Use ps aux to count processes
    const char* cmd = "ps aux --no-heading | wc -l";
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    
    if (!pipe) throw std::runtime_error("popen failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return std::stoi(result); // Convert string to integer
}

std::string get_server2_info() {
    std::ostringstream info;
    info << "Thread count: " << get_thread_count() << "\n";
    info << "Process count: " << get_process_count();
    return info.str();
}