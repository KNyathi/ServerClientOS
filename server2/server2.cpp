// server2.cpp
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <atomic>
#include <csignal>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include "system_info.h" // Include system info function declarations
#include <ctime>

std::string get_current_time() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct); // Up to seconds
    return std::string(buf);
}

#define PORT 5002      // Server2 port
#define LOG_PORT 6000  // Logging server port

std::atomic<bool> running(true);  // Flag to control shutdown

void send_log_to_server(const std::string& log_message) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "SERVER2|Socket creation error for log server\n";
        return;
    }

    struct sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(LOG_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "SERVER2|Invalid address\n";
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "SERVER2|Connection to log server failed\n";
        close(sock);
        return;
    }

    send(sock, log_message.c_str(), log_message.length(), 0);
    close(sock);
}

// Catch termination signal
void handle_signal(int signum) {
    send_log_to_server("SERVER2|Server2 stopped by signal");
    running = false;
    std::cout << "\nSERVER2|Shutting down...\n";
    exit(0);  // Force exit to stop blocking accept()
}

void client_handler(int client_socket) {
    char buffer[1024] = {0};
    int read_size = read(client_socket, buffer, sizeof(buffer));
    if (read_size <= 0) {
        send_log_to_server("SERVER2|Error reading client request");
        close(client_socket);
        return;
    }

    std::string request(buffer);

    if (request == "INFO") {
        int thread_count = get_thread_count();
        int process_count = get_process_count();
        std::string response =  "Time: " +  get_current_time() +
                                "\nThread count: " + std::to_string(thread_count) +
                               "\nProcess count: " + std::to_string(process_count);
        send(client_socket, response.c_str(), response.length(), 0);
        send_log_to_server("SERVER2|Sent system info to client");
    }

    send_log_to_server("SERVER2|Client request received: " + request);
    close(client_socket);
}

void start_server() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr {}, client_addr {};
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        send_log_to_server("SERVER2|Socket creation failed");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        send_log_to_server("SERVER2|Bind failed");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 5) < 0) {
        send_log_to_server("SERVER2|Listen failed");
        close(server_fd);
        return;
    }

    send_log_to_server("SERVER2|Server2 started");
    std::cout << "SERVER2|Server2 is listening on port " << PORT << "\n";

    while (running) {
        client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            if (running)
                send_log_to_server("SERVER2|Accept failed");
            continue;
        }

        send_log_to_server("SERVER2|Client connected");
        std::thread(client_handler, client_socket).detach();
    }

    close(server_fd);
    send_log_to_server("SERVER2|Server2 stopped");
}

int main() {
    signal(SIGINT, handle_signal);   // Ctrl+C
    signal(SIGTERM, handle_signal);  // kill
    start_server();
    return 0;
}