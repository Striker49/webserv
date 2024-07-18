#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h> 
#include <iostream>
#include <cstring>

class Socket {
public:
    Socket();
    Socket(const Socket &other);
    Socket& operator=(const Socket &other);
    ~Socket();

    bool create();
    bool bind(int port);
    bool listen(int backlog);
    bool set_non_blocking();
    int accept();
    bool connect(const std::string &address, int port);
    bool set_option(int level, int option_name, int option_value);
    int get_socket_fd() const;
    bool set_reuse_address();
    void close_socket();

    static Socket* create_server_socket();
    int get_config_index() const;
    void set_config_index(int index);

private:
    int sock_fd;
    int config_index;
};

#endif
