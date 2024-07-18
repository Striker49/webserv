#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include "../debug/colors.hpp"

enum log_level {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

class logger {
public:
    logger(const std::string &log_file);
    logger(const logger &other);
    logger &operator=(const logger &other);
    ~logger();

    void log(log_level level, const std::string &message);
    void debug(const std::string &message);
    void info(const std::string &message);
    void warning(const std::string &message);
    void error(const std::string &message);

private:
    std::ofstream log_stream;
    std::string log_file;
    std::string get_color(log_level level);
    std::string get_level_string(log_level level);
};

extern logger log_instance;
void log_and_print_shutdown_message();
void log_configuration_success();
void log_new_connection(int client_sock);
bool log_and_return(const std::string &message, bool return_value);
void log_request(int client_sock, const std::string &total_request);
void log_response_status(int client_sock, ssize_t total_sent, size_t response_length);
void log_server_setup(const server_config& config, int port);
void log_response(const std::string& response_str);

#endif
