// server2.cpp
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <sys/ptrace.h>
#include "system_info.h" // Include the header for system info functions

#define PORT 5002  // Port for server2
#define LOG_PORT 6000  // Log server port

void send_log_to_server(const std::string& log_message) {
    int sock = 0;
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error for log server\n";
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(LOG_PORT);  // Log server on port 6000

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection to log server failed\n";
        return;
    }

    send(sock, log_message.c_str(), log_message.length(), 0);
    close(sock);
}

// Function to handle client requests
void client_handler(int client_socket) {
    char buffer[1024] = {0};
    int read_size;

    // Read the client's request
    read_size = read(client_socket, buffer, sizeof(buffer));
    if (read_size <= 0) {
        std::cerr << "Error reading client request\n";
        close(client_socket);
        return;
    }

    std::string request(buffer);

    if (request == "INFO") {
        // Send system information to the client
        int thread_count = get_thread_count();
        int process_count = get_process_count();
        std::string response = "Thread count: " + std::to_string(thread_count) + "\nProcess count: " + std::to_string(process_count);

        send(client_socket, response.c_str(), response.length(), 0);
    }

    // Log client request
    send_log_to_server("Client connected, Request: " + request);

    close(client_socket);
}

void start_server() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed\n";
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed\n";
        return;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed\n";
        return;
    }

    // Log server start
    send_log_to_server("Server2 started");

    std::cout << "Server2 is listening on port " << PORT << "\n";

    // Handle clients in separate threads
    while (true) {
        client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        // Log client connection
        send_log_to_server("Client connected");

        // Handle the client in a new thread
        std::thread(client_handler, client_socket).detach();
    }

    close(server_fd);
}

int main() {
    start_server();
    return 0;
}