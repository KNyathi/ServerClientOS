#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT1 5001  // Port for server1
#define PORT2 5002  // Port for server2

void fetch_server1_info() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[2048] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error\n";
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT1);  // server1 on port 5001

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return;
    }

    const char* message = "INFO";
    send(sock, message, strlen(message), 0);

    read(sock, buffer, sizeof(buffer));
    std::cout << "[Client] Server1 Info:\n" << buffer << "\n";

    close(sock);
}

void fetch_server2_info() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[2048] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error\n";
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT2);  // server2 on port 5002

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return;
    }

    const char* message = "INFO";
    send(sock, message, strlen(message), 0);

    read(sock, buffer, sizeof(buffer));
    std::cout << "[Client] Server2 Info:\n" << buffer << "\n";

    close(sock);
}

int main() {
    // Fetch system info from both servers
    fetch_server1_info();
    fetch_server2_info();

    return 0;
}