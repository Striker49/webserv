#include "../../include/webserv.hpp"

Socket::Socket() : sock_fd(-1), config_index(-1) {}

Socket::Socket(const Socket &other) : sock_fd(other.sock_fd), config_index(other.config_index) {}

Socket& Socket::operator=(const Socket &other) {
    if (this != &other) {
        sock_fd = other.sock_fd;
        config_index = other.config_index;
    }
    return *this;
}

Socket::~Socket() {
    if (sock_fd != -1) {
        close_socket();
    }
}

bool Socket::create() {
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        std::string error_message = "Failed to create socket: " + std::string(strerror(errno));
        log_instance.error(error_message);
        print_message(STDERR_FILENO, error_message, COLOR_RED);
        return false;
    }
    log_instance.info("Socket created successfully, fd: " + std::to_string(sock_fd));
    return true;
}

bool Socket::bind(int port) {
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (::bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::string error_message = "Failed to bind socket: " + std::string(strerror(errno));
        log_instance.error(error_message);
        print_message(STDERR_FILENO, error_message, COLOR_RED);
        return false;
    }
    log_instance.info("Socket bound to port: " + std::to_string(port));
    return true;
}

bool Socket::listen(int backlog) {
    if (::listen(sock_fd, backlog) == -1) {
        std::string error_message = "Failed to listen on socket: " + std::string(strerror(errno));
        log_instance.error(error_message);
        print_message(STDERR_FILENO, error_message, COLOR_RED);
        return false;
    }
    log_instance.info("Socket is listening with backlog: " + std::to_string(backlog));
    return true;
}

bool Socket::set_non_blocking() {
    int flags = fcntl(sock_fd, F_GETFL, 0);
    if (flags == -1) {
        std::string error_message = "Failed to get socket flags: " + std::string(strerror(errno));
        log_instance.error(error_message);
        print_message(STDERR_FILENO, error_message, COLOR_RED);
        return false;
    }
    if (fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::string error_message = "Failed to set socket non-blocking: " + std::string(strerror(errno));
        log_instance.error(error_message);
        print_message(STDERR_FILENO, error_message, COLOR_RED);
        return false;
    }
    log_instance.info("Socket set to non-blocking mode, fd: " + std::to_string(sock_fd));
    return true;
}

int Socket::accept() {
    int client_sock = ::accept(sock_fd, NULL, NULL);
    if (client_sock == -1) {
        int error_code = errno;
        if (error_code == EAGAIN || error_code == EWOULDBLOCK) {
            //log_instance.debug("Non-blocking socket accept returned EAGAIN or EWOULDBLOCK. This is expected behavior.");
        } else {
            std::string error_message = "Failed to accept connection: " + std::string(strerror(errno));
            log_instance.error(error_message);
            print_message(STDERR_FILENO, error_message, COLOR_RED);
        }
    } else {
        log_instance.info("Accepted new connection, client socket fd: " + std::to_string(client_sock));
    }
    return client_sock;
}

bool Socket::connect(const std::string &address, int port) {
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, address.c_str(), &addr.sin_addr) <= 0) {
        std::string error_message = "Invalid address: " + std::string(strerror(errno));
        log_instance.error(error_message);
        print_message(STDERR_FILENO, error_message, COLOR_RED);
        return false;
    }

    if (::connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::string error_message = "Failed to connect: " + std::string(strerror(errno));
        log_instance.error(error_message);
        print_message(STDERR_FILENO, error_message, COLOR_RED);
        return false;
    }
    log_instance.info("Connected to " + address + " on port " + std::to_string(port));
    return true;
}

bool Socket::set_option(int level, int option_name, int option_value) {
    if (setsockopt(sock_fd, level, option_name, &option_value, sizeof(option_value)) == -1) {
        std::string error_message = "Failed to set socket option: " + std::string(strerror(errno));
        log_instance.error(error_message);
        print_message(STDERR_FILENO, error_message, COLOR_RED);
        return false;
    }
    log_instance.info("Set socket option " + std::to_string(option_name) + " to value " + std::to_string(option_value));
    return true;
}

bool Socket::set_reuse_address() {
    int yes = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        std::string error_message = "Error setting SO_REUSEADDR: " + std::string(strerror(errno));
        log_instance.error(error_message);
        print_message(STDERR_FILENO, error_message, COLOR_RED);
        return false;
    }
    log_instance.info("Socket set to reuse address, fd: " + std::to_string(sock_fd));
    return true;
}

int Socket::get_socket_fd() const {
    return sock_fd;
}

Socket* Socket::create_server_socket() {
    return new Socket();
}

void Socket::set_config_index(int index) {
    config_index = index;
}

int Socket::get_config_index() const {
    return config_index;
}

void Socket::close_socket() {
    if (sock_fd != -1) {
        ::close(sock_fd);
        log_instance.info("Closed socket, fd: " + std::to_string(sock_fd));
        sock_fd = -1;
    }
}
