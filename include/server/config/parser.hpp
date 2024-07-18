#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <map>

class server_config;

enum token_type {
    TOKEN_TEXT,
    TOKEN_DELIMITER
};

struct token {
    token_type type;
    std::string value;
};

std::vector<token> tokenize(const std::string &config_content);
bool validate_tokens(const std::vector<token>& tokens, std::vector<server_config>& configs);

#endif 