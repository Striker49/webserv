#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include "colors.hpp"

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

#endif // LOGGER_HPP

