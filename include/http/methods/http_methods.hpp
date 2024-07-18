#ifndef HTTP_METHODS_HPP
#define HTTP_METHODS_HPP

#include <string>
#include <vector>
#include <algorithm>

class http_methods {
public:
    http_methods();
    http_methods(const http_methods& other);
    http_methods& operator=(const http_methods& other);
    ~http_methods();

    bool is_valid_method(const std::string& method, const std::vector<std::string>& allowed_methods) const;
    void add_method(std::vector<std::string>& allowed_methods, const std::string& method);
    void remove_method(std::vector<std::string>& allowed_methods, const std::string& method);
};

#endif
