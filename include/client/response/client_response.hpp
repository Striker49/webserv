#ifndef CLIENT_RESPONSE_HPP
#define CLIENT_RESPONSE_HPP

#include "../request/client_request.hpp"
#include <string>
#include <map>

class client_response {
public:
    client_response();
    client_response(const client_response& other);
    client_response& operator=(const client_response& other);
    ~client_response();

    void handle_request(const client_request& request);
    std::string generate_response() const;

private:
    std::string status_line;
    std::map<std::string, std::string> headers;
    std::string body;

    std::string get_mime_type(const std::string& path) const;
    void set_default_headers();
    void copy_from(const client_response& other);
};

#endif // CLIENT_RESPONSE_HPP
