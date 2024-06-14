#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include "../config/config_parser.hpp"

class server_config {
public:
    server_config();
    server_config(const server_config &other);
    server_config &operator=(const server_config &other);
    ~server_config();

    void set_listen(int port);
    int get_listen() const;

    void set_server_name(const std::string &name);
    const std::string &get_server_name() const;

    void set_root(const std::string &path);
    const std::string &get_root() const;

    void add_location(const location_config &location);
    const std::vector<location_config>& get_locations() const;

    void set_client_max_body_size(int size);
    int get_client_max_body_size() const;

    void set_error_pages(int code, const std::string &path);
    const std::string &get_error_page(int code) const;

    void set_upload_path(const std::string &path);
    const std::string &get_upload_path() const;

    void set_allowed_methods(const std::vector<std::string> &methods);
    const std::vector<std::string>& get_allowed_methods() const;

private:
    int listen_port;
    std::string server_name;
    std::string root;
    std::vector<location_config> locations;
    int client_max_body_size;
    std::map<int, std::string> error_pages;
    std::string upload_path;
    std::vector<std::string> allowed_methods;
};


std::vector<token> tokenize(const std::string &config_content);
bool validate_tokens(const std::vector<token>& tokens, std::vector<server_config>& configs);
void print_tokens(const std::vector<token>& tokens);
void print_server_configs(const std::vector<server_config>& configs);

#endif // SERVER_HPP