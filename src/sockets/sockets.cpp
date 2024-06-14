#include "../../include/webserv.hpp"

Socket::Socket() : socket_fd(-1) {
    memset(&address, 0, sizeof(address));
}

Socket::Socket(const Socket &other) : socket_fd(other.socket_fd), address(other.address) {}

Socket &Socket::operator=(const Socket &other) {
    if (this != &other) {
        socket_fd = other.socket_fd;
        address = other.address;
    }
    return *this;
}

Socket::~Socket() {
    close_socket();
}

bool Socket::create() {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::cerr << "Failed to create socket\n";
        return false;
    }
    std::cout << "Socket created: " << socket_fd << std::endl;
    return true;
}

bool Socket::bind(int port) {
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (::bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind socket\n";
        return false;
    }
    std::cout << "Socket bound: " << socket_fd << " to port " << port << std::endl;
    return true;
}

bool Socket::listen(int backlog) {
    if (::listen(socket_fd, backlog) < 0) {
        std::cerr << "Failed to listen on socket\n";
        return false;
    }
    std::cout << "Listening on socket: " << socket_fd << std::endl;
    return true;
}

int Socket::accept() {
    int addr_len = sizeof(address);
    int client_fd = ::accept(socket_fd, (struct sockaddr *)&address, (socklen_t *)&addr_len);
    if (client_fd < 0) {
        std::cerr << "Failed to accept connection\n";
        return -1;
    }
    std::cout << "Accepted connection: " << client_fd << std::endl;
    return client_fd;
}

void Socket::close_socket() {
    if (socket_fd != -1) {
        std::cout << "Closing socket: " << socket_fd << std::endl;
        close(socket_fd);
        socket_fd = -1;
    }
}

bool Socket::set_non_blocking() {
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Failed to get socket flags\n";
        return false;
    }

    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "Failed to set socket to non-blocking\n";
        return false;
    }

    std::cout << "Socket set to non-blocking: " << socket_fd << std::endl;
    return true;
}

int Socket::get_socket_fd() const {
    return socket_fd;
}
