#include "../../../include/webserv.hpp"

server_config::server_config() : client_max_body_size(0), autoindex(false) {
}

server_config::server_config(const server_config &other) {
    *this = other;
}

server_config &server_config::operator=(const server_config &other) {
    if (this != &other) {
        listen_ports = other.listen_ports;
        server_name = other.server_name;
        root = other.root;
        client_max_body_size = other.client_max_body_size;
        error_pages = other.error_pages;
        upload_path = other.upload_path;
        default_methods = other.default_methods;
        locations = other.locations;
        autoindex = other.autoindex;
    }
    return *this;
}

server_config::~server_config() {}

void server_config::add_listen(int port) {
    listen_ports.push_back(port);
}

const std::vector<int>& server_config::get_listen() const {
    return listen_ports;
}

void server_config::set_server_name(const std::string &name) {
    server_name = name;
}

const std::string &server_config::get_server_name() const {
    return server_name;
}

void server_config::set_root(const std::string &root) {
    this->root = root;
}

const std::string &server_config::get_root() const {
    return root;
}

void server_config::set_client_max_body_size(size_t size) {
    client_max_body_size = size;
}

size_t server_config::get_client_max_body_size() const {
    return client_max_body_size;
}

void server_config::set_error_pages(int code, const std::string &path) {
    error_pages[code] = path;
}

const std::map<int, std::string> &server_config::get_error_pages() const {
    return error_pages;
}

void server_config::set_upload_path(const std::string &path) {
    upload_path = path;
}

const std::string &server_config::get_upload_path() const {
    return upload_path;
}

void server_config::set_default_methods(std::string &method) {
    default_methods.push_back(method);
}

const std::vector<std::string> &server_config::get_default_methods() const {
    return default_methods;
}

void server_config::add_location(const location_config &location) {
    locations.push_back(location);
}

const std::vector<location_config> &server_config::get_locations() const {
    return locations;
}

void server_config::set_autoindex(bool autoindex) {
    this->autoindex = autoindex;
}

bool server_config::get_autoindex() const {
    return autoindex;
}
