#include "../../include/webserv.hpp"

SocketManager::SocketManager() : max_fd(0) {
    FD_ZERO(&master_set);
    std::cout << "SocketManager constructed" << std::endl;
}

SocketManager::SocketManager(const SocketManager &other) : sockets(other.sockets), master_set(other.master_set), max_fd(other.max_fd) {
    std::cout << "SocketManager copy constructed" << std::endl;
}

SocketManager &SocketManager::operator=(const SocketManager &other) {
    if (this != &other) {
        sockets = other.sockets;
        master_set = other.master_set;
        max_fd = other.max_fd;
    }
    std::cout << "SocketManager assigned" << std::endl;
    return *this;
}

SocketManager::~SocketManager() {
    std::cout << "SocketManager destructor called, closing all sockets" << std::endl;
    for (std::vector<Socket>::iterator it = sockets.begin(); it != sockets.end(); ++it) {
        it->close_socket();
    }
}

bool SocketManager::add_socket(int port) {
    Socket new_socket;
    if (!new_socket.create() || !new_socket.bind(port) || !new_socket.listen(10) || !new_socket.set_non_blocking()) {
        return false;
    }

    sockets.push_back(new_socket);
    FD_SET(new_socket.get_socket_fd(), &master_set);
    if (new_socket.get_socket_fd() > max_fd) {
        max_fd = new_socket.get_socket_fd();
    }
    std::cout << "Socket added to manager: " << new_socket.get_socket_fd() << std::endl;
    return true;
}

void SocketManager::run() {
    fd_set read_set;
    std::cout << "Entering SocketManager::run()" << std::endl;
    while (true) {
        read_set = master_set;

        // Log the state of file descriptors before select
        std::cout << "File descriptors before select: ";
        for (int fd = 0; fd <= max_fd; ++fd) {
            if (FD_ISSET(fd, &master_set)) {
                std::cout << fd << " ";
            }
        }
        std::cout << std::endl;

        std::cout << "Waiting on select..." << std::endl;
        int select_result = select(max_fd + 1, &read_set, NULL, NULL, NULL);
        if (select_result < 0) {
            if (errno == EINTR) {
                continue; // Interrupted by signal, continue loop
            }
            std::cerr << "Select error: " << std::strerror(errno) << std::endl;
            return;
        }

        std::cout << "Select returned, processing fds..." << std::endl;
        for (int fd = 0; fd <= max_fd; ++fd) {
            if (FD_ISSET(fd, &read_set)) {
                if (is_listening_socket(fd)) {
                    handle_new_connection(fd);
                } else {
                    handle_client_message(fd);
                }
            }
        }
    }
    std::cout << "Exiting SocketManager::run()" << std::endl;
}

void SocketManager::handle_new_connection(int socket_fd) {
    for (std::vector<Socket>::iterator it = sockets.begin(); it != sockets.end(); ++it) {
        if (it->get_socket_fd() == socket_fd) {
            int client_fd = it->accept();
            if (client_fd != -1) {
                FD_SET(client_fd, &master_set);
                if (client_fd > max_fd) {
                    max_fd = client_fd;
                }
                std::cout << "New connection accepted, client_fd: " << client_fd << std::endl;
            }
            return;
        }
    }
}

void SocketManager::handle_client_message(int client_fd) {
    char buffer[1024];
    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            std::cout << "Client disconnected, client_fd: " << client_fd << std::endl;
        } else {
            std::cerr << "Recv error: " << std::strerror(errno) << std::endl;
        }
        close(client_fd);
        FD_CLR(client_fd, &master_set);
    } else {
        buffer[bytes_received] = '\0';
        std::cout << "Received message from client_fd " << client_fd << ": " << buffer << std::endl;
    }
}

bool SocketManager::is_listening_socket(int fd) {
    for (std::vector<Socket>::iterator it = sockets.begin(); it != sockets.end(); ++it) {
        if (it->get_socket_fd() == fd) {
            return true;
        }
    }
    return false;
}

