#include "../../../include/webserv.hpp"

// Constructor
server_config::server_config()
    : listen_port(0), client_max_body_size(0) {}

// Copy Constructor
server_config::server_config(const server_config &other)
    : listen_port(other.listen_port), server_name(other.server_name), root(other.root),
      locations(other.locations), client_max_body_size(other.client_max_body_size),
      error_pages(other.error_pages), upload_path(other.upload_path),
      allowed_methods(other.allowed_methods) {}

// Assignment Operator
server_config &server_config::operator=(const server_config &other) {
    if (this != &other) {
        listen_port = other.listen_port;
        server_name = other.server_name;
        root = other.root;
        locations = other.locations;
        client_max_body_size = other.client_max_body_size;
        error_pages = other.error_pages;
        upload_path = other.upload_path;
        allowed_methods = other.allowed_methods;
    }
    return *this;
}

// Destructor
server_config::~server_config() {}

// Set and Get functions for listen_port
void server_config::set_listen(int port) {
    listen_port = port;
}

int server_config::get_listen() const {
    return listen_port;
}

// Set and Get functions for server_name
void server_config::set_server_name(const std::string &name) {
    server_name = name;
}

const std::string &server_config::get_server_name() const {
    return server_name;
}

// Set and Get functions for root
void server_config::set_root(const std::string &path) {
    root = path;
}

const std::string &server_config::get_root() const {
    return root;
}

// Add and Get functions for locations
void server_config::add_location(const location_config &location) {
    locations.push_back(location);
}

const std::vector<location_config>& server_config::get_locations() const {
    return locations;
}

// Set and Get functions for client_max_body_size
void server_config::set_client_max_body_size(int size) {
    client_max_body_size = size;
}

int server_config::get_client_max_body_size() const {
    return client_max_body_size;
}

// Set and Get functions for error_pages
void server_config::set_error_pages(int code, const std::string &path) {
    error_pages[code] = path;
}

const std::string &server_config::get_error_page(int code) const {
    return error_pages.at(code);
}

// Set and Get functions for upload_path
void server_config::set_upload_path(const std::string &path) {
    upload_path = path;
}

const std::string &server_config::get_upload_path() const {
    return upload_path;
}

// Set and Get functions for allowed_methods
void server_config::set_allowed_methods(const std::vector<std::string> &methods) {
    allowed_methods = methods;
}

const std::vector<std::string>& server_config::get_allowed_methods() const {
    return allowed_methods;
}
