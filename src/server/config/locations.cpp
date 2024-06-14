#include "../../../include/webserv.hpp"

// Constructor
location_config::location_config() : autoindex(false) {}

// Copy Constructor
location_config::location_config(const location_config &other)
    : path(other.path), root(other.root), index(other.index), autoindex(other.autoindex), cgi(other.cgi) {}

// Assignment Operator
location_config &location_config::operator=(const location_config &other) {
    if (this != &other) {
        path = other.path;
        root = other.root;
        index = other.index;
        autoindex = other.autoindex;
        cgi = other.cgi;
    }
    return *this;
}

// Destructor
location_config::~location_config() {}

// Set and Get functions for path
void location_config::set_path(const std::string &path) {
    this->path = path;
}

const std::string &location_config::get_path() const {
    return path;
}

// Set and Get functions for root
void location_config::set_root(const std::string &root) {
    this->root = root;
}

const std::string &location_config::get_root() const {
    return root;
}

// Set and Get functions for index
void location_config::set_index(const std::string &index) {
    this->index = index;
}

const std::string &location_config::get_index() const {
    return index;
}

// Set and Get functions for autoindex
void location_config::set_autoindex(bool autoindex) {
    this->autoindex = autoindex;
}

bool location_config::get_autoindex() const {
    return autoindex;
}

// Add and Get functions for CGI
void location_config::add_cgi(const std::string &extension, const std::string &path) {
    cgi[extension] = path;
}

const std::map<std::string, std::string> &location_config::get_cgi() const {
    return cgi;
}

