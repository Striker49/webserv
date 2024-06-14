#ifndef SOCKET_MANAGER_HPP
#define SOCKET_MANAGER_HPP

#include "sockets.hpp"
#include <vector>

class Socket;

class SocketManager {
public:
    SocketManager();
    SocketManager(const SocketManager &other);
    SocketManager &operator=(const SocketManager &other);
    ~SocketManager();

    bool add_socket(int port);
    void run();

private:
    std::vector<Socket> sockets;
    fd_set master_set;
    int max_fd;

    void handle_new_connection(int socket_fd);
    void handle_client_message(int client_fd);
    bool is_listening_socket(int fd);
};

#endif // SOCKET_MANAGER_HPP

