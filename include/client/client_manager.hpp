#ifndef CLIENT_MANAGER_HPP
#define CLIENT_MANAGER_HPP

#include "connection/client_connection.hpp"
#include <vector>
#include <sys/select.h>

class client_manager {
public:
    client_manager();
    client_manager(const client_manager& other);
    client_manager& operator=(const client_manager& other);
    ~client_manager();

    void add_client(int client_socket);
    void handle_clients();

private:
    std::vector<client_connection*> clients;
    fd_set master_set;
    int max_fd;

    void remove_client(client_connection* client);
    void copy_from(const client_manager& other);
};

#endif // CLIENT_MANAGER_HPP
