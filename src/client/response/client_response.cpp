#include "../../../include/webserv.hpp"

// Utility function to check if a string ends with a given suffix
bool ends_with(const std::string& value, const std::string& ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

client_response::client_response() {}

client_response::client_response(const client_response& other) {
    copy_from(other);
}

client_response& client_response::operator=(const client_response& other) {
    if (this != &other) {
        copy_from(other);
    }
    return *this;
}

client_response::~client_response() {}

void client_response::handle_request(const client_request& request) {
    // Validate HTTP version
    if (request.get_http_version() != "HTTP/1.1") {
        status_line = "HTTP/1.1 505 HTTP Version Not Supported";
        body = "Only HTTP/1.1 is supported";
        set_default_headers();
        return;
    }

    // For simplicity, we will handle only GET requests
    if (request.get_method() != "GET") {
        status_line = "HTTP/1.1 405 Method Not Allowed";
        body = "Only GET method is supported";
        set_default_headers();
        return;
    }

    // Determine the file path and MIME type
    std::string path = request.get_path();
    std::string mime_type = get_mime_type(path);

    // For now, let's assume we have the file content hardcoded
    if (path == "/index.html") {
        body = "<html><body><h1>Welcome to the Server</h1></body></html>";
    } else if (path == "/styles.css") {
        body = "body { font-family: Arial; }";
    } else if (path == "/script.js") {
        body = "console.log('Hello, world!');";
    } else {
        status_line = "HTTP/1.1 404 Not Found";
        body = "File not found";
        set_default_headers();
        return;
    }

    status_line = "HTTP/1.1 200 OK";
    headers["Content-Type"] = mime_type;
    headers["Content-Length"] = std::to_string(body.size());
    set_default_headers();
}

std::string client_response::generate_response() const {
    std::ostringstream response_stream;

    // Construct status line
    response_stream << status_line << "\r\n";

    // Construct headers
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        response_stream << it->first << ": " << it->second << "\r\n";
    }
    response_stream << "\r\n"; // End of headers

    // Construct body
    response_stream << body;

    return response_stream.str();
}

std::string client_response::get_mime_type(const std::string& path) const {
    if (ends_with(path, ".html")) {
        return "text/html";
    } else if (ends_with(path, ".css")) {
        return "text/css";
    } else if (ends_with(path, ".js")) {
        return "application/javascript";
    } else {
        return "application/octet-stream";
    }
}

void client_response::set_default_headers() {
    headers["Server"] = "CustomServer/1.0";
    headers["Connection"] = "close";
}

void client_response::copy_from(const client_response& other) {
    status_line = other.status_line;
    headers = other.headers;
    body = other.body;
}
