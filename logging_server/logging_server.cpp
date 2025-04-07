#include <iostream>
#include <fstream>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <ctime>
#include <sstream>

#define LOG_PORT 6000

std::string get_current_time() {
    time_t now = time(0);
    char buf[80];
    strftime(buf, sizeof(buf), "[%Y-%m-%d %H:%M:%S]", localtime(&now));
    return std::string(buf);
}

void log_to_file(const std::string& filename, const std::string& message) {
    std::ofstream logfile(filename, std::ios::app);
    logfile << get_current_time() << " - " << message << std::endl;
}

void handle_log_client(int client_socket) {
    char buffer[1024] = {0};
    int valread = read(client_socket, buffer, sizeof(buffer));
    if (valread > 0) {
        std::string raw_message(buffer);
        std::string filename = "general_logs.txt";  // fallback

        if (raw_message.rfind("SERVER1|", 0) == 0) {
            filename = "server1_logs.txt";
            raw_message = raw_message.substr(8);  // Remove "SERVER1|"
        } else if (raw_message.rfind("SERVER2|", 0) == 0) {
            filename = "server2_logs.txt";
            raw_message = raw_message.substr(8);  // Remove "SERVER2|"
        }

        log_to_file(filename, raw_message);
        std::cout << "Logged to " << filename << ": " << raw_message << std::endl;
    }

    close(client_socket);
}

void start_logging_server() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(LOG_PORT);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    std::cout << "Logging server started on port " << LOG_PORT << std::endl;

    while (true) {
        client_socket = accept(server_fd, (struct sockaddr*)&server_addr, &addr_len);
        std::thread(handle_log_client, client_socket).detach();
    }

    close(server_fd);
}

int main() {
    start_logging_server();
    return 0;
}