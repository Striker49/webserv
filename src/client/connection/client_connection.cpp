#include "../../../include/webserv.hpp"

client_connection::client_connection() : client_socket(-1) {}

client_connection::client_connection(int socket) : client_socket(socket) {
    client_addr_len = sizeof(client_address);
    getpeername(client_socket, (struct sockaddr*)&client_address, &client_addr_len);
}

client_connection::client_connection(const client_connection& other) {
    copy_from(other);
}

client_connection& client_connection::operator=(const client_connection& other) {
    if (this != &other) {
        close_connection();
        copy_from(other);
    }
    return *this;
}

client_connection::~client_connection() {
    close_connection();
}

void client_connection::handle_request() {
    char buffer[1024];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

    if (bytes_received < 0) {
        std::cerr << "Error receiving data" << std::endl;
        return;
    }

    buffer[bytes_received] = '\0';
    std::string request_data(buffer);

    // Parse the request
    request.parse_request(request_data);

    // Handle the request and generate response
    response.handle_request(request);

    // Send the response
    std::string response_data = response.generate_response();
    send(client_socket, response_data.c_str(), response_data.size(), 0);

    close_connection();
}

void client_connection::close_connection() {
    if (client_socket != -1) {
        close(client_socket);
        client_socket = -1;
    }
}

int client_connection::get_client_socket() const {
    return client_socket;
}

void client_connection::copy_from(const client_connection& other) {
    client_socket = other.client_socket;
    client_address = other.client_address;
    client_addr_len = other.client_addr_len;
    request = other.request;
    response = other.response;
}
