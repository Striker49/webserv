#ifndef CLIENT_CONNECTION_HPP
#define CLIENT_CONNECTION_HPP

#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../request/client_request.hpp"
#include "../response/client_response.hpp"

class client_connection {
public:
    client_connection();
    client_connection(int client_socket);
    client_connection(const client_connection& other);
    client_connection& operator=(const client_connection& other);
    ~client_connection();

    void handle_request();
    int get_client_socket() const;

private:
    int client_socket;
    sockaddr_in client_address;
    socklen_t client_addr_len;
    client_request request;
    client_response response;

    void close_connection();
    void copy_from(const client_connection& other);
};

#endif // CLIENT_CONNECTION_HPP

