#include "../../../include/webserv.hpp"

http_response::http_response() : http_version_("HTTP/1.1"), status_code_(200), status_message_("OK") {}

std::string http_response::build_response() const {
    std::ostringstream response_stream;
    response_stream << http_version_ << " " << status_code_ << " " << status_message_ << "\r\n";
    
    for (std::map<std::string, std::string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it) {
        response_stream << it->first << ": " << it->second << "\r\n";
    }
    
    response_stream << "\r\n" << body_;
    return response_stream.str();
}

const std::string& http_response::get_http_version() const {
    return http_version_;
}

int http_response::get_status_code() const {
    return status_code_;
}

const std::string& http_response::get_status_message() const {
    return status_message_;
}

const std::map<std::string, std::string>& http_response::get_headers() const {
    return headers_;
}

const std::string& http_response::get_body() const {
    return body_;
}

void http_response::set_http_version(const std::string& http_version) {
    http_version_ = http_version;
}

void http_response::set_status_code(int status_code) {
    status_code_ = status_code;
    status_message_ = status_code_to_message(status_code);
}

void http_response::set_status_message(const std::string& status_message) {
    status_message_ = status_message;
}

void http_response::add_header(const std::string& key, const std::string& value) {
    headers_[key] = value;
}

void http_response::set_body(const std::string& body) {
    body_ = body;
}

std::string http_response::status_code_to_message(int status_code) {
    switch (status_code) {
        case 200: return "OK";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Request Entity Too Large";
        case 415: return "Unsupported Media Type";
        case 500: return "Internal Server Error";
        case 505: return "HTTP Version not supported";
        default: return "Unknown Status";
    }
}
