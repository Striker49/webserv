#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <map>

class http_response {
public:
    http_response();
    std::string build_response() const;

    const std::string& get_http_version() const;
    int get_status_code() const;
    const std::string& get_status_message() const;
    const std::map<std::string, std::string>& get_headers() const;
    const std::string& get_body() const;

    void set_http_version(const std::string& http_version);
    void set_status_code(int status_code);
    void set_status_message(const std::string& status_message);
    void add_header(const std::string& key, const std::string& value);
    void set_body(const std::string& body);
    std::string status_code_to_message(int status_code);

private:
    std::string http_version_;
    int status_code_;
    std::string status_message_;
    std::map<std::string, std::string> headers_;
    std::string body_;
};

#endif
