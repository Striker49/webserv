#ifndef SOCKETS_HPP
#define SOCKETS_HPP

#include <netinet/in.h>
#include <sys/select.h>
#include "socket_manager.hpp"

class Socket {
public:
    Socket();
    Socket(const Socket &other);
    Socket &operator=(const Socket &other);
    ~Socket();

    bool create();
    bool bind(int port);
    bool listen(int backlog = 5);
    int accept();
    void close_socket();
    bool set_non_blocking();

    int get_socket_fd() const;

private:
    int socket_fd;
    struct sockaddr_in address;
};

#endif // SOCKETS_HPP
