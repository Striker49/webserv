#include "../../include/webserv.hpp"

client_manager::client_manager() : max_fd(0) {
    FD_ZERO(&master_set);
}

client_manager::client_manager(const client_manager& other) {
    copy_from(other);
}

client_manager& client_manager::operator=(const client_manager& other) {
    if (this != &other) {
        for (size_t i = 0; i < clients.size(); ++i) {
            delete clients[i];
        }
        copy_from(other);
    }
    return *this;
}

client_manager::~client_manager() {
    for (size_t i = 0; i < clients.size(); ++i) {
        delete clients[i];
    }
}

void client_manager::add_client(int client_socket) {
    client_connection* new_client = new client_connection(client_socket);
    clients.push_back(new_client);
    FD_SET(client_socket, &master_set);
    if (client_socket > max_fd) {
        max_fd = client_socket;
    }
    std::cout << "Client added: " << client_socket << std::endl;
}

void client_manager::handle_clients() {
    fd_set read_set = master_set;
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int select_result = select(max_fd + 1, &read_set, NULL, NULL, &timeout);
    if (select_result < 0) {
        std::cerr << "Select error: " << std::strerror(errno) << std::endl;
        return;
    }

    for (int fd = 0; fd <= max_fd; ++fd) {
        if (FD_ISSET(fd, &read_set)) {
            for (size_t i = 0; i < clients.size(); ++i) {
                if (clients[i]->get_client_socket() == fd) {
                    clients[i]->handle_request();
                    remove_client(clients[i]);
                    break;
                }
            }
        }
    }
}

void client_manager::remove_client(client_connection* client) {
    int client_socket = client->get_client_socket();
    FD_CLR(client_socket, &master_set);
    if (client_socket == max_fd) {
        max_fd = -1;
        for (size_t i = 0; i < clients.size(); ++i) {
            if (clients[i]->get_client_socket() > max_fd) {
                max_fd = clients[i]->get_client_socket();
            }
        }
    }
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    delete client;
    std::cout << "Client removed: " << client_socket << std::endl;
}

void client_manager::copy_from(const client_manager& other) {
    max_fd = other.max_fd;
    master_set = other.master_set;
    for (size_t i = 0; i < other.clients.size(); ++i) {
        clients.push_back(new client_connection(*other.clients[i]));
    }
}
