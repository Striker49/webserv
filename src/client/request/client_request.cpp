#include "../../../include/webserv.hpp"

client_request::client_request() {}

client_request::client_request(const client_request& other) {
    copy_from(other);
}

client_request& client_request::operator=(const client_request& other) {
    if (this != &other) {
        copy_from(other);
    }
    return *this;
}

client_request::~client_request() {}

void client_request::parse_request(const std::string& request_data) {
    std::istringstream request_stream(request_data);
    std::string line;

    // Parse the start line
    if (std::getline(request_stream, line)) {
        parse_start_line(line);
    }

    // Parse headers
    std::string headers_part;
    while (std::getline(request_stream, line) && line != "\r") {
        headers_part += line + "\n";
    }
    parse_headers(headers_part);

    // Parse body
    std::string body_part;
    while (std::getline(request_stream, line)) {
        body_part += line + "\n";
    }
    parse_body(body_part);
}

void client_request::parse_start_line(const std::string& start_line) {
    std::istringstream start_line_stream(start_line);
    start_line_stream >> method >> path >> http_version;

    if (http_version != "HTTP/1.1") {
        throw std::runtime_error("Invalid HTTP version");
    }
}

void client_request::parse_headers(const std::string& headers_part) {
    std::istringstream headers_stream(headers_part);
    std::string line;
    while (std::getline(headers_stream, line)) {
        std::string::size_type separator = line.find(':');
        if (separator != std::string::npos) {
            std::string key = line.substr(0, separator);
            std::string value = line.substr(separator + 1);
            headers[key] = value;
        }
    }
}

void client_request::parse_body(const std::string& body_part) {
    body = body_part;
}

std::string client_request::get_method() const {
    return method;
}

std::string client_request::get_path() const {
    return path;
}

std::string client_request::get_http_version() const {
    return http_version;
}

std::map<std::string, std::string> client_request::get_headers() const {
    return headers;
}

std::string client_request::get_body() const {
    return body;
}

void client_request::copy_from(const client_request& other) {
    method = other.method;
    path = other.path;
    http_version = other.http_version;
    headers = other.headers;
    body = other.body;
}