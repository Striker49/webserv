#include "../../include/webserv.hpp"

client::client() {}

client::client(const client& other) : manager(other.manager) {}

client& client::operator=(const client& other) {
    if (this != &other) {
        manager = other.manager;
    }
    return *this;
}

client::~client() {}

void client::run(const std::string& server_address, int server_port) {
    int server_socket = create_socket(server_address, server_port);
    if (server_socket < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    manager.add_client(server_socket);

    while (true) {
        manager.handle_clients();
    }
}

void client::connect_to_server(const std::string& server_address, int server_port) {
    int server_socket = create_socket(server_address, server_port);
    if (server_socket < 0) {
        std::cerr << "Failed to connect to server" << std::endl;
        return;
    }

    manager.add_client(server_socket);
}

int client::create_socket(const std::string& server_address, int server_port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_address.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        close(sockfd);
        return -1;
    }

    return sockfd;
}
