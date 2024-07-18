#include "../../../include/webserv.hpp"

location_config::location_config() : autoindex(false) {}

location_config::location_config(const location_config &other)
    : path(other.path), root(other.root), index(other.index), autoindex(other.autoindex), cgi(other.cgi), allowed_methods(other.allowed_methods) {}

location_config &location_config::operator=(const location_config &other) {
    if (this != &other) {
        path = other.path;
        root = other.root;
        index = other.index;
        autoindex = other.autoindex;
        cgi = other.cgi;
        allowed_methods = other.allowed_methods;
    }
    return *this;
}

location_config::~location_config() {}

void location_config::set_path(const std::string &path) {
    this->path = path;
}

const std::string &location_config::get_path() const {
    return path;
}

void location_config::set_root(const std::string &root) {
    this->root = root;
}

const std::string &location_config::get_root() const {
    return root;
}

void location_config::set_index(const std::string &index) {
    this->index = index;
}

const std::string &location_config::get_index() const {
    return index;
}

void location_config::set_autoindex(bool autoindex) {
    this->autoindex = autoindex;
}

bool location_config::get_autoindex() const {
    return autoindex;
}

void location_config::add_cgi(const std::string &extension, const std::string &path) {
    cgi[extension] = path;
}

const std::map<std::string, std::string> &location_config::get_cgi() const {
    return cgi;
}

void location_config::set_allowed_methods(const std::vector<std::string> &methods) {
    allowed_methods = methods;
}

const std::vector<std::string> &location_config::get_allowed_methods() const {
    return allowed_methods;
}
