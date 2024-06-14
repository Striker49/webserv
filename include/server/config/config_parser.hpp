#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <vector>
#include <map>
#include "../core/server.hpp"

enum token_type {
    TOKEN_TEXT,
    TOKEN_DELIMITER
};

struct token {
    token_type type;
    std::string value;
};

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

private:
    std::string path;
    std::string root;
    std::string index;
    bool autoindex;
    std::map<std::string, std::string> cgi;
};

// std::vector<token> tokenize(const std::string &config_content);
// bool validate_tokens(const std::vector<token>& tokens, std::vector<server_config>& configs);
// void print_tokens(const std::vector<token>& tokens);

// // Function prototypes
// bool is_delimiter(char c);
// void create_token(std::vector<token>& tokens, token_type type, const std::string& value);
// bool handle_server_block(bool& in_server_block);
// bool handle_opening_brace(bool in_server_block, int& brace_balance);
// bool handle_closing_brace(int& brace_balance, bool& in_server_block);
// bool parse_listen(const std::string& value, server_config& config);
// bool parse_server_name(const std::string& value, server_config& config);
// bool parse_root_server(const std::string& value, server_config& config);
// bool parse_root_location(const std::string& value, location_config& config);
// bool parse_index(const std::string& value, location_config& config);
// bool parse_max_body_size(const std::string& value, server_config& config);
// bool parse_error_pages(const std::string& value, server_config& config);
// bool parse_upload_path(const std::string& value, server_config& config);
// bool parse_allowed_methods(const std::string& value, server_config& config);
// bool parse_autoindex(const std::string& value, location_config& config);
// bool parse_cgi(const std::string& value, location_config& config);
// bool extract_key_value(const std::vector<token>& tokens, std::vector<token>::const_iterator& it, std::string& key, std::string& value);
// bool parse_key_value_pair(const std::vector<token>& tokens, std::vector<token>::const_iterator& it, server_config& config);
// bool parse_location_key_value_pair(const std::vector<token>& tokens, std::vector<token>::const_iterator& it, location_config& config);
// bool parse_location_block(const std::vector<token>& tokens, std::vector<token>::const_iterator& it, location_config& config);
// bool parse_server_block(const std::vector<token>& tokens, std::vector<token>::const_iterator& it, server_config& config);

#endif // TOKEN_HPP
