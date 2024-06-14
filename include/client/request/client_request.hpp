#ifndef CLIENT_REQUEST_HPP
#define CLIENT_REQUEST_HPP

#include <string>
#include <map>

class client_request {
public:
    client_request();
    client_request(const client_request& other);
    client_request& operator=(const client_request& other);
    ~client_request();

    void parse_request(const std::string& request_data);

    std::string get_method() const;
    std::string get_path() const;
    std::string get_http_version() const;
    std::map<std::string, std::string> get_headers() const;
    std::string get_body() const;

private:
    std::string method;
    std::string path;
    std::string http_version;
    std::map<std::string, std::string> headers;
    std::string body;

    void parse_start_line(const std::string& start_line);
    void parse_headers(const std::string& headers_part);
    void parse_body(const std::string& body_part);
    void copy_from(const client_request& other);
};

#endif // CLIENT_REQUEST_HPP

