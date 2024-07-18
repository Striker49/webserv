#include "../../../include/webserv.hpp"

extern logger log_instance;

bool is_delimiter(char c) {
    return (c == '{' || c == '}' || c == ';' || c == '=');
}

void create_token(std::vector<token>& tokens, token_type type, const std::string& value) {
    token tok;
    tok.type = type;
    tok.value = value;
    tokens.push_back(tok);

    std::string token_type_str;
    if (type == TOKEN_DELIMITER) {
        token_type_str = "delimiter";
    } else {
        token_type_str = "text";
    }
    log_instance.debug("Tokenized " + token_type_str + ": " + tok.value);
}

bool handle_server_block(bool& in_server_block) {
    if (in_server_block) {
        log_instance.error("Nested server blocks are not allowed");
        return false;
    }
    in_server_block = true;
    log_instance.debug("Found server block");
    return true;
}

bool handle_opening_brace(bool in_server_block, int& brace_balance) {
    if (!in_server_block || brace_balance != 0) {
        log_instance.error("Invalid opening brace position");
        return false;
    }
    ++brace_balance;
    log_instance.debug("Opening brace found");
    return true;
}

bool handle_closing_brace(int& brace_balance, bool& in_server_block) {
    if (brace_balance <= 1) {
        in_server_block = false;
        log_instance.debug("Closing brace found, ending server block");
    } else {
        log_instance.debug("Closing brace found");
    }
    --brace_balance;
    return brace_balance >= 0;
}

bool parse_listen(const std::string& value, server_config& config) {
    try {
        std::istringstream port_stream(value);
        std::string port;
        while (std::getline(port_stream, port, ',')) {
            port = trim_whitespace(port); 
            int port_num = std::stoi(port);

            if (port_num < 0 || port_num > 65535) {
                log_instance.error("Invalid port number (out of range): " + port);
                return false;
            }

            config.add_listen(port_num);
            log_instance.debug("Parsed listen port: " + port);
        }
    } catch (const std::invalid_argument& e) {
        log_instance.error("Invalid port number (not a number): " + value);
        return false;
    } catch (const std::out_of_range& e) {
        log_instance.error("Invalid port number (out of range): " + value);
        return false;
    } catch (...) {
        log_instance.error("Unknown error while parsing port number: " + value);
        return false;
    }
    return true;
}

bool parse_server_name(const std::string& value, server_config& config) {
    config.set_server_name(value);
    log_instance.debug("Parsed server name: " + value);
    return true;
}

bool parse_root_server(const std::string& value, server_config& config) {
    config.set_root(value);
    log_instance.debug("Parsed root directory: " + value);
    return true;
}

bool parse_root_location(const std::string& value, location_config& config) {
    config.set_root(value);
    log_instance.debug("Parsed root directory: " + value);
    return true;
}

bool parse_index(const std::string& value, location_config& config) {
    config.set_index(value);
    log_instance.debug("Parsed index file: " + value);
    return true;
}

size_t convert_letter_to_zeros(std::string value, size_t pos)
{
    if (value[pos] == 'K')
        return (1000);
    if (value[pos] == 'M')
        return (1000000);
    if (value[pos] == 'G')
        return (1000000000);
    return (1);
}

bool parse_max_body_size(const std::string& value, server_config& config) {
    try {
        size_t size = std::stoul(value);
        if (value.find_first_not_of("0123456789", 0) != std::string::npos)
        {
            size *= convert_letter_to_zeros(value, value.find_first_not_of("0123456789", 0));
        }
        config.set_client_max_body_size(size);
        log_instance.debug("Parsed max body size: " + value);
    } catch (...) {
        log_instance.error("Invalid max body size: " + value);
        return false;
    }
    return true;
}

void trim(std::string &str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    if (first == std::string::npos || last == std::string::npos) {
        str = "";
    } else {
        str = str.substr(first, (last - first + 1));
    }
}

bool parse_error_pages(const std::string& value, server_config& config) {
    std::istringstream stream(value);
    std::string item;
    while (std::getline(stream, item, ';')) {
        std::istringstream item_stream(item);
        std::string code_str, path;
        if (!(item_stream >> code_str >> path)) {
            log_instance.error("Invalid error page format: " + item);
            return false;
        }
        trim(code_str);
        trim(path);
        int code;
        try {
            code = std::stoi(code_str);
            config.set_error_pages(code, path);
            log_instance.debug("Parsed error page - Code: " + code_str + ", Path: " + path);
        } catch (...) {
            log_instance.error("Invalid error page code or format in item: " + item);
            return false;
        }
    }
    return true;
}

bool parse_upload_path(const std::string& value, server_config& config) {
    config.set_upload_path(value);
    log_instance.debug("Parsed upload path: " + value);
    return true;
}

bool parse_allowed_methods(const std::string& value, location_config& config) {
    std::istringstream stream(value);
    std::vector<std::string> methods;
    std::string method;
    while (stream >> method) {
        methods.push_back(method);
    }
    config.set_allowed_methods(methods);
    log_instance.debug("Parsed allowed methods for location: " + value);
    return true;
}

bool parse_default_methods(const std::string& value, server_config& config) {
    std::istringstream stream(value);
    std::vector<std::string> methods;
    std::string method;
    while (stream >> method) {
        methods.push_back(method);
        config.set_default_methods(method);
    }
    log_instance.debug("Parsed default methods: " + value);
    return true;
}

bool parse_autoindex_server(const std::string& value, server_config& config) {
    bool autoindex = (value == "true");
    config.set_autoindex(autoindex);
    log_instance.debug("Parsed server level autoindex: " + std::to_string(autoindex));
    return true;
}

bool parse_autoindex(const std::string& value, location_config& config) {
    bool autoindex = (value == "true");
    config.set_autoindex(autoindex);
    log_instance.debug("Parsed autoindex: " + value);
    return true;
}

bool parse_cgi(const std::string& value, location_config& config) {
    size_t comma_pos = value.find(',');
    if (comma_pos == std::string::npos) {
        log_instance.error("Invalid CGI format: " + value);
        return false;
    }
    try {
        std::string extension = value.substr(0, comma_pos);
        std::string path = value.substr(comma_pos + 1);
        config.add_cgi(extension, path);
        log_instance.debug("Parsed CGI: " + value);
    } catch (...) {
        log_instance.error("Invalid CGI configuration: " + value);
        return false;
    }
    return true;
}

bool extract_key_value(const std::vector<token>& tokens, std::vector<token>::const_iterator& it, std::string& key, std::string& value) {
    if (it == tokens.end() || it->type != TOKEN_TEXT) {
        log_instance.error("Expected key in server block");
        return false;
    }

    key = it->value;
    ++it;

    if (it == tokens.end() || it->value != "=") {
        log_instance.error("Expected '=' after key: " + key);
        return false;
    }
    ++it;

    if (it == tokens.end() || it->type != TOKEN_TEXT) {
        log_instance.error("Expected value after '=' for key: " + key);
        return false;
    }

    value = it->value;
    ++it;

    while (it != tokens.end() && it->value != ";") {
        if (it->value != ",") {
            value += " " + it->value;
        }
        ++it;
    }

    if (it == tokens.end() || it->value != ";") {
        log_instance.error("Expected ';' after value for key: " + key);
        return false;
    }
    ++it;

    return true;
}

bool parse_key_value_pair(const std::vector<token>& tokens, std::vector<token>::const_iterator& it, server_config& config) {
    std::string key;
    std::string value;

    if (!extract_key_value(tokens, it, key, value)) {
        return false;
    }

    if (key == "listen") {
        if (!parse_listen(value, config)) {
            return false;
        }
    } else if (key == "server_name") {
        if (!parse_server_name(value, config)) {
            return false;
        }
    } else if (key == "root") {
        if (!parse_root_server(value, config)) {
            return false;
        }
    } else if (key == "max_body_size") {
        if (!parse_max_body_size(value, config)) {
            return false;
        }
    } else if (key == "error_pages") {
        if (!parse_error_pages(value, config)) {
            return false;
        }
    } else if (key == "upload_path") {
        if (!parse_upload_path(value, config)) {
            return false;
        }
    } else if (key == "default_methods") {
        if (!parse_default_methods(value, config)) {
            return false;
        }
    } else if (key == "autoindex") {
        if (!parse_autoindex_server(value, config)) {
            return false;
        }
    } else {
        log_instance.error("Unknown key in server block: " + key);
        return false;
    }

    return true;
}

bool parse_location_key_value_pair(const std::vector<token>& tokens, std::vector<token>::const_iterator& it, location_config& config) {
    std::string key;
    std::string value;

    if (!extract_key_value(tokens, it, key, value)) {
        return false;
    }

    if (key == "root") {
        if (!parse_root_location(value, config)) {
            return false;
        }
    } else if (key == "index") {
        if (!parse_index(value, config)) {
            return false;
        }
    } else if (key == "autoindex") {
        if (!parse_autoindex(value, config)) {
            return false;
        }
    } else if (key == "cgi") {
        if (!parse_cgi(value, config)) {
            return false;
        }
    } else if (key == "allowed_methods") {
        if (!parse_allowed_methods(value, config)) {
            return false;
        }
    } else {
        log_instance.error("Unknown key in location block: " + key);
        return false;
    }

    return true;
}

bool parse_location_block(const std::vector<token>& tokens, std::vector<token>::const_iterator& it, location_config& config) {
    while (it != tokens.end() && it->value != "}") {
        if (!parse_location_key_value_pair(tokens, it, config)) {
            return false;
        }
    }
    return true;
}

bool parse_server_block(const std::vector<token>& tokens, std::vector<token>::const_iterator& it, server_config& config) {
    while (it != tokens.end() && it->value != "}") {
        if (it->type == TOKEN_TEXT && it->value == "location") {
            ++it;
            if (it == tokens.end() || it->type != TOKEN_TEXT) {
                log_instance.error("Expected path after 'location'");
                return false;
            }

            location_config loc_config;
            loc_config.set_path(it->value);
            ++it;

            if (it == tokens.end() || it->value != "{") {
                log_instance.error("Expected '{' after location path");
                return false;
            }
            ++it;

            if (!parse_location_block(tokens, it, loc_config)) {
                return false;
            }

            config.add_location(loc_config);

            if (it == tokens.end() || it->value != "}") {
                log_instance.error("Expected '}' at the end of location block");
                return false;
            }
            ++it;
        } else {
            if (!parse_key_value_pair(tokens, it, config)) {
                return false;
            }
        }
    }
    return true;
}

std::vector<token> tokenize(const std::string &config_content) {
    std::vector<token> tokens;
    std::string::const_iterator it = config_content.begin();

    while (it != config_content.end()) {
        if (std::isspace(*it)) {
            ++it;
            continue;
        }

        if (is_delimiter(*it)) {
            create_token(tokens, TOKEN_DELIMITER, std::string(1, *it));
            ++it;
        } else {
            std::string text;
            while (it != config_content.end() && !std::isspace(*it) && !is_delimiter(*it)) {
                text.push_back(*it);
                ++it;
            }
            create_token(tokens, TOKEN_TEXT, text);
        }
    }

    log_instance.info("Tokenization completed successfully");
    return tokens;
}

void print_tokens(const std::vector<token>& tokens) {
    for (std::vector<token>::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
        std::cout << "Type: " << (it->type == TOKEN_DELIMITER ? "Delimiter" : "Text") << ", Value: " << it->value << std::endl;
    }
}
bool validate_semicolons(const std::vector<token>& tokens) {
    for (std::vector<token>::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
        if (it->type == TOKEN_TEXT) {
            std::vector<token>::const_iterator next_it = it + 1;
            if (next_it != tokens.end() && next_it->value == "=") {
                next_it++;
                while (next_it != tokens.end() && next_it->type != TOKEN_DELIMITER) {
                    ++next_it;
                }
                if (next_it == tokens.end() || next_it->value != ";") {
                    log_instance.error("Expected ';' after key-value pair: " + it->value);
                    return false;
                }
                it = next_it;
            }
        }
    }
    return true;
}

bool validate_tokens(const std::vector<token>& tokens, std::vector<server_config>& configs) {
    int brace_balance = 0;
    bool in_server_block = false;

    // Validate semicolons first
    if (tokens.empty())
        return false;
    if (!validate_semicolons(tokens)) {
        return false;
    }

    for (std::vector<token>::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
        if (it->type == TOKEN_TEXT && it->value == "server") {
            if (!handle_server_block(in_server_block)) {
                return false;
            }
            it++;
            if (it == tokens.end() || it->value != "{") {
                log_instance.error("Expected '{' after 'server'");
                return false;
            }
            if (!handle_opening_brace(in_server_block, brace_balance)) {
                return false;
            }
            it++;
            server_config config;
            if (!parse_server_block(tokens, it, config)) {
                return false;
            }
            configs.push_back(config);
            if (it == tokens.end() || it->value != "}") {
                log_instance.error("Expected '}' at the end of server block");
                return false;
            }
            if (!handle_closing_brace(brace_balance, in_server_block)) {
                return false;
            }
        }
    }
    bool valid = (brace_balance == 0);
    if (valid) {
        log_instance.info("Validation successful");
    } else {
        log_instance.error("Validation failed");
    }
    return valid;
}
