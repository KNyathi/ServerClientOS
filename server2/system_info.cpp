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
    
    if (!pipe) {
        throw std::runtime_error("popen failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    // If no result was fetched, handle the case
    if (result.empty()) {
        throw std::runtime_error("Failed to read process count");
    }

    try {
        return std::stoi(result); // Convert string to integer
    } catch (const std::invalid_argument& e) {
        throw std::runtime_error("Invalid output from ps aux: " + result);
    }
}

std::string get_server2_info() {
    std::ostringstream info;
    info << "Thread count: " << get_thread_count() << "\n";
    try {
        info << "Process count: " << get_process_count();
    } catch (const std::exception& e) {
        info << "\nError fetching process count: " << e.what();
    }
    return info.str();
}