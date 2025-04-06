#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>

#define LOG_PORT 6000

void log_event(const std::string& message) {
    // Open the log file for appending
    std::ofstream log_file("server_logs.txt", std::ios_base::app);
    if (!log_file) {
        std::cerr << "Error opening log file.\n";
        return;
    }

    // Get the current time
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm* local_time = std::localtime(&now);
    char time_buffer[20];
    std::strftime(time_buffer, sizeof(time_buffer), "%F %T", local_time);

    // Write to the log
    log_file << "[" << time_buffer << "] - " << message << std::endl;
}

void start_logging_server() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed\n";
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(LOG_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed\n";
        return;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed\n";
        return;
    }

    std::cout << "Logging server started on port " << LOG_PORT << "\n";

    while (true) {
        client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        char buffer[1024] = {0};
        int bytes_received = read(client_socket, buffer, sizeof(buffer));

        if (bytes_received > 0) {
            std::string log_message(buffer, bytes_received);
            log_event(log_message);  // Log the received message
        }

        close(client_socket);
    }

    close(server_fd);
}

int main() {
    start_logging_server();
    return 0;
}