#ifndef LOCATIONS_HPP
#define LOCATIONS_HPP

#include <string>
#include <vector>
#include <map>

class location_config {
public:
    location_config();
    location_config(const location_config &other);
    location_config &operator=(const location_config &other);
    ~location_config();

    void set_path(const std::string &path);
    const std::string &get_path() const;

    void set_root(const std::string &root);
    const std::string &get_root() const;

    void set_index(const std::string &index);
    const std::string &get_index() const;

    void set_autoindex(bool autoindex);
    bool get_autoindex() const;

    void add_cgi(const std::string &extension, const std::string &path);
    const std::map<std::string, std::string> &get_cgi() const;

    void set_allowed_methods(const std::vector<std::string> &methods);
    const std::vector<std::string> &get_allowed_methods() const;

private:
    std::string path;
    std::string root;
    std::string index;
    bool autoindex;
    std::map<std::string, std::string> cgi;
    std::vector<std::string> allowed_methods;
};

#endif
