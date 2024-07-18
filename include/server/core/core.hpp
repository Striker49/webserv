#ifndef CORE_HPP
#define CORE_HPP

#include <string>
#include <vector>
#include <map>

enum status_code {
    DEFAULT                      = 99,
    OK                           = 200,
    CREATED                      = 201,
    ACCEPTED                     = 202,
    NO_CONTENT                   = 204,
    MOVED_PERMANENTLY            = 301,
    FOUND                        = 302,
    SEE_OTHER                    = 303,
    NOT_MODIFIED                 = 304,
    BAD_REQUEST                  = 400,
    UNAUTHORIZED                 = 401,
    PAYMENT_REQUIRED             = 402,
    FORBIDDEN                    = 403,
    NOT_FOUND                    = 404,
    METHOD_NOT_ALLOWED           = 405,
    CONTENT_TOO_LARGE            = 413,
    UNSUPPORTED_MEDIA_TYPE       = 415,
    IM_A_TEAPOT                  = 418,
    INTERNAL_SERVER_ERROR        = 500,
    NOT_IMPLEMENTED              = 501,
    BAD_GATEWAY                  = 502,
    SERVICE_UNAVAILABLE          = 503,
    GATEWAY_TIMEOUT              = 504,
    HTTP_VERSION_NOT_SUPPORTED   = 505
};

class location_config;

class server_config {
public:
    server_config();
    server_config(const server_config &other);
    server_config &operator=(const server_config &other);
    ~server_config();

    void add_listen(int port);
    const std::vector<int>& get_listen() const;

    void set_server_name(const std::string &name);
    const std::string &get_server_name() const;

    void set_root(const std::string &root);
    const std::string &get_root() const;

    void set_client_max_body_size(size_t size);
    size_t get_client_max_body_size() const;

    void set_error_pages(int code, const std::string &path);
    const std::map<int, std::string> &get_error_pages() const;

    void set_upload_path(const std::string &path);
    const std::string &get_upload_path() const;

    void add_location(const location_config &location);
    const std::vector<location_config> &get_locations() const;

    void set_autoindex(bool autoindex);
    bool get_autoindex() const;

    void set_default_methods(std::string &method);
    const std::vector<std::string>& get_default_methods() const;

private:
    std::vector<int> listen_ports;
    std::string server_name;
    std::string root;
    size_t client_max_body_size;
    std::map<int, std::string> error_pages;
    std::string upload_path;
    std::vector<location_config> locations;
    bool autoindex;
    std::vector<std::string> default_methods;
};

#endif
