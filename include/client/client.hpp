
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "connection/client_connection.hpp"
#include "request/client_request.hpp"
#include "response/client_response.hpp"
#include "client_manager.hpp"


class client {
public:
    client();
    client(const client& other);
    client& operator=(const client& other);
    ~client();

    void run(const std::string& server_address, int server_port);

private:
    client_manager manager;

    void connect_to_server(const std::string& server_address, int server_port);
    int create_socket(const std::string& server_address, int server_port);
};

#endif // CLIENT_HPP
