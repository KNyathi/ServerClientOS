#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <csignal>
#include <atomic>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <map>

extern std::string get_window_size();
extern int get_monitor_count();
extern std::string get_current_time();

#define PORT 5001         // Port for server1
#define LOG_PORT 6000     // Log server port

std::atomic<bool> running(true);  // Flag to control server loop

std::map<std::string, int> client_ids; // Map to store IP + Port as the key and Client ID as the value
int next_client_id = 1; // Track the next available client ID

// Function to send logs to another server
void send_log_to_server(const std::string& log_message) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "SERVER1|Socket creation error for log server\n";
        return;
    }

    struct sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(LOG_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "SERVER1|Invalid address for log server\n";
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "SERVER1|Connection to log server failed\n";
        close(sock);
        return;
    }

    send(sock, log_message.c_str(), log_message.length(), 0);
    close(sock);
}

// Signal handler for graceful shutdown
void handle_signal(int signum) {
    send_log_to_server("SERVER1|Server1 stopped by signal");
    running = false;
    std::cout << "\nSERVER1|Server1 is shutting down...\n";
    exit(0);
}

// Function to handle each client request
void client_handler(int client_socket, struct sockaddr_in client_addr) {
    char buffer[1024] = {0};
    int read_size = read(client_socket, buffer, sizeof(buffer));
    if (read_size <= 0) {
        if (read_size == 0) {
            std::cerr << "SERVER1|Client disconnected\n";
            send_log_to_server("SERVER1|Client disconnected\n");
        } else {
            std::cerr <<"SERVER1|Error reading client request\n";
            send_log_to_server("SERVER1|Error reading client request");
        }
       
        close(client_socket);
        return;
    }

    // Get the client's IP and port as a unique identifier
    std::string client_key = std::to_string(client_addr.sin_addr.s_addr) + ":" + std::to_string(client_addr.sin_port);

    // Assign a client ID if it's a new connection, otherwise reuse the existing ID
    if (client_ids.find(client_key) == client_ids.end()) {
        client_ids[client_key] = next_client_id++;
    }

    int client_id = client_ids[client_key];
    std::string request(buffer);

    if (request == "INFO") {
        std::string window_size = get_window_size();
        int monitor_count = get_monitor_count();
        std::string response = "Client " + std::to_string(client_id) + " - Time: " + get_current_time() + 
                               "\nWindow size: " + window_size + "\nMonitors: " + std::to_string(monitor_count);
        send(client_socket, response.c_str(), response.length(), 0);
        send_log_to_server("SERVER1|Sent system info to client " + std::to_string(client_id));
    }

    send_log_to_server("SERVER1|Client " + std::to_string(client_id) + " request received: " + request);
    
}

// Main function to start the server
void start_server() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr {}, client_addr {};
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        send_log_to_server("SERVER1|Socket creation failed");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        send_log_to_server("SERVER1|Bind failed");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 5) < 0) {
        send_log_to_server("SERVER1|Listen failed");
        close(server_fd);
        return;
    }

    send_log_to_server("SERVER1|Server1 started");
    std::cout << "SERVER1|Server1 is listening on port " <<PORT << "\n";

    while (running) {
        client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            if (running)  // Only log accept failure if not shutting down
                send_log_to_server("SERVER1|Accept failed");
            continue;
        }

        send_log_to_server("SERVER1|Client connected");
        std::thread(client_handler, client_socket, client_addr).detach();
    }

    close(server_fd);
    send_log_to_server("SERVER1|Server1 stopped");
}

int main() {
    signal(SIGINT, handle_signal);   // Catch Ctrl+C
    signal(SIGTERM, handle_signal);  // Catch kill
    start_server();
    return 0;
}