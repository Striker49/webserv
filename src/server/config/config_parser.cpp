#include "../../../include/webserv.hpp"

extern logger log_instance;

// Function to check if a character is a delimiter
bool is_delimiter(char c) {
    return (c == '{' || c == '}' || c == ';' || c == '=');
}

// Function to create and add a token to the vector
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


// Helper function to manage the `server` block state
bool handle_server_block(bool& in_server_block) {
    if (in_server_block) {
        log_instance.error("Nested server blocks are not allowed");
        return false;
    }
    in_server_block = true;
    log_instance.debug("Found server block");
    return true;
}

// Helper function to manage the opening brace
bool handle_opening_brace(bool in_server_block, int& brace_balance) {
    if (!in_server_block || brace_balance != 0) {
        log_instance.error("Invalid opening brace position");
        return false;
    }
    ++brace_balance;
    log_instance.debug("Opening brace found");
    return true;
}

// Helper function to manage the closing brace
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

// Helper function to parse the `listen` key
bool parse_listen(const std::string& value, server_config& config) {
    try {
        int port = std::stoi(value);
        config.set_listen(port);
        log_instance.debug("Parsed listen port: " + value);
    } catch (...) {
        log_instance.error("Invalid port number: " + value);
        return false;
    }
    return true;
}

// Helper function to parse the `server_name` key
bool parse_server_name(const std::string& value, server_config& config) {
    config.set_server_name(value);
    log_instance.debug("Parsed server name: " + value);
    return true;
}

// Helper function to parse the `root` key in server block
bool parse_root_server(const std::string& value, server_config& config) {
    config.set_root(value);
    log_instance.debug("Parsed root directory: " + value);
    return true;
}

// Helper function to parse the `root` key in location block
bool parse_root_location(const std::string& value, location_config& config) {
    config.set_root(value);
    log_instance.debug("Parsed root directory: " + value);
    return true;
}

// Helper function to parse the `index` key
bool parse_index(const std::string& value, location_config& config) {
    config.set_index(value);
    log_instance.debug("Parsed index file: " + value);
    return true;
}

// Helper function to parse the `max_body_size` key
bool parse_max_body_size(const std::string& value, server_config& config) {
    try {
        size_t size = std::stoul(value);
        config.set_client_max_body_size(size);
        log_instance.debug("Parsed max body size: " + value);
    } catch (...) {
        log_instance.error("Invalid max body size: " + value);
        return false;
    }
    return true;
}

// Helper function to parse the `error_pages` key
bool parse_error_pages(const std::string& value, server_config& config) {
    std::istringstream stream(value);
    std::string item;
    while (std::getline(stream, item, ',')) {
        item = item.substr(item.find_first_not_of(" \t"));
        size_t pos = item.find(' ');
        if (pos == std::string::npos) {
            log_instance.error("Invalid error page format: " + item);
            return false;
        }
        try {
            int code = std::stoi(item.substr(0, pos));
            std::string path = item.substr(pos + 1);
            path = path.substr(path.find_first_not_of(" \t"));
            config.set_error_pages(code, path);
            log_instance.debug("Parsed error page: " + item);
        } catch (...) {
            log_instance.error("Invalid error page code: " + item);
            return false;
        }
    }
    return true;
}

// Helper function to parse the `upload_path` key
bool parse_upload_path(const std::string& value, server_config& config) {
    config.set_upload_path(value);
    log_instance.debug("Parsed upload path: " + value);
    return true;
}

// Helper function to parse the `allowed_methods` key
bool parse_allowed_methods(const std::string& value, server_config& config) {
    std::istringstream stream(value);
    std::vector<std::string> methods;
    std::string method;
    while (stream >> method) {
        methods.push_back(method);
    }
    config.set_allowed_methods(methods);
    log_instance.debug("Parsed allowed methods: " + value);
    return true;
}

// Helper function to parse the `autoindex` key
bool parse_autoindex(const std::string& value, location_config& config) {
    bool autoindex = (value == "true");
    config.set_autoindex(autoindex);
    log_instance.debug("Parsed autoindex: " + value);
    return true;
}

// Helper function to parse the `cgi` key
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

// Helper function to extract a key-value pair from tokens
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

    // Check for multi-value entries separated by commas
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

// Helper function to parse a key-value pair in the server block
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
    } else if (key == "allowed_methods") {
        if (!parse_allowed_methods(value, config)) {
            return false;
        }
    } else {
        log_instance.error("Unknown key in server block: " + key);
        return false;
    }

    return true;
}

// Helper function to parse a key-value pair in the location block
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
    } else {
        log_instance.error("Unknown key in location block: " + key);
        return false;
    }

    return true;
}

// Helper function to parse location block content
bool parse_location_block(const std::vector<token>& tokens, std::vector<token>::const_iterator& it, location_config& config) {
    while (it != tokens.end() && it->value != "}") {
        if (!parse_location_key_value_pair(tokens, it, config)) {
            return false;
        }
    }
    return true;
}

// Helper function to parse server block content
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

// Main tokenization function
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

// Print tokens 
void print_tokens(const std::vector<token>& tokens) {
    for (std::vector<token>::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
        std::cout << "Type: " << (it->type == TOKEN_DELIMITER ? "Delimiter" : "Text") << ", Value: " << it->value << std::endl;
    }
}

// Validate tokens to ensure correct configuration format
bool validate_tokens(const std::vector<token>& tokens, std::vector<server_config>& configs) {
    int brace_balance = 0;
    bool in_server_block = false;

    for (std::vector<token>::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
        if (it->type == TOKEN_TEXT && it->value == "server") {
            if (!handle_server_block(in_server_block)) {
                return false;
            }
            ++it;
            if (it == tokens.end() || it->value != "{") {
                log_instance.error("Expected '{' after 'server'");
                return false;
            }
            if (!handle_opening_brace(in_server_block, brace_balance)) {
                return false;
            }
            ++it;

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
